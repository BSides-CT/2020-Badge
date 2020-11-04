#include <atmel_start.h>
#include "application.h"
#include "cards/cards.h"

static ISO14443A_Card iso14443a_card;
static ISO14443B_Card iso14443b_card;
static ISO18092_Card iso18092_card;

static uint8_t rx_data[256];

static CR95HF_Version ic_version = CR95HF_VERSION_QJE;
static CR95HF_Protocol cur_protocol = CR95HF_PROTOCOL_OFF;

static void CR95HF_extint(void)
{

}

static void CR95HF_sendCommand(uint8_t *data)
{
	SPI_start();

	SPI_write(CR95HF_COMMAND_SEND);

	if (data[CR95HF_COMMAND_OFFSET] == CR95HF_COMMAND_ECHO)
	{
		SPI_write(CR95HF_COMMAND_ECHO);
	}
	else
	{
		SPI_writeBuffer(data, data[CR95HF_LENGTH_OFFSET] + CR95HF_DATA_OFFSET);
	}

	SPI_end();
}

static void CR95HF_getResponse(void)
{
	SPI_start();

	SPI_write(CR95HF_COMMAND_RECEIVE);

	// Read command byte
	rx_data[CR95HF_COMMAND_OFFSET] = SPI_read();

	if (rx_data[CR95HF_COMMAND_OFFSET] == CR95HF_COMMAND_ECHO ||
	rx_data[CR95HF_COMMAND_OFFSET] == 0xFF)
	{
		rx_data[CR95HF_LENGTH_OFFSET] = 0x00;
		SPI_readBuffer(&rx_data[CR95HF_LENGTH_OFFSET + 1], 2);
	}
	else
	{
		// Read length byte
		rx_data[CR95HF_LENGTH_OFFSET] = SPI_read();
		// Read response data
		SPI_readBuffer(&rx_data[CR95HF_DATA_OFFSET], rx_data[CR95HF_LENGTH_OFFSET]);
	}

	SPI_end();
}

static uint8_t CR95HF_poll(void)
{
	uint32_t start = millis();
	uint8_t status = 0;

	SPI_start();

	do
	{
		delay_ms(2);

		status = SPI_transfer(CR95HF_COMMAND_POLLING);
		status &= CR95HF_FLAG_DATA_READY_MASK;

		if ((millis() - start) > 1000)
		{
			SPI_end();
			return 1;
		}
	}
	while (status != CR95HF_FLAG_DATA_READY);

	SPI_end();
	return 0;
}

static uint8_t CR95HF_error(uint8_t command)
{
	switch (command)
	{
		case CR95HF_COMMAND_SENDRECV:
		if ((rx_data[CR95HF_COMMAND_OFFSET] == 0x80) &&
		(rx_data[CR95HF_COMMAND_OFFSET + 1] != 0))
		{
			return 0;
		} else if (rx_data[CR95HF_COMMAND_OFFSET] == 0x90)
		{
			return 0;
		}
		case CR95HF_COMMAND_IDN:
		case CR95HF_PROTOCOL:
		case CR95HF_COMMAND_IDLE:
		case CR95HF_COMMAND_RDREG:
		case CR95HF_COMMAND_WRREG:
		if (rx_data[CR95HF_COMMAND_OFFSET] == 0x00)
		{
			return 0;
		}
		case CR95HF_COMMAND_ECHO:
		if (rx_data[CR95HF_COMMAND_OFFSET] == CR95HF_COMMAND_ECHO)
		{
			return 0;
		}
	}

	return 1;
}

uint8_t CR95HF_sendReceive(uint8_t *data)
{
	CR95HF_sendCommand(data);

	if (CR95HF_poll() != 0)
	{
		rx_data[CR95HF_COMMAND_OFFSET] = CR95HF_POLLING_TIMEOUT;
		return 1;
	}

	CR95HF_getResponse();

	return CR95HF_error(data[CR95HF_DATA_OFFSET]);
}

static uint8_t CR95HF_writeRegister(uint8_t length, uint8_t address, uint8_t flags, uint8_t *data)
{
	uint8_t buffer[257];

	buffer[CR95HF_COMMAND_OFFSET] = CR95HF_COMMAND_WRREG;
	buffer[CR95HF_LENGTH_OFFSET] = length;
	buffer[CR95HF_DATA_OFFSET] = address;
	buffer[CR95HF_DATA_OFFSET + 1] = flags;

	if ((length > 2) && data)
	{
		memcpy(&buffer[CR95HF_DATA_OFFSET + 2], data, length - 2);
	}

	return CR95HF_sendReceive(buffer);
}

static void CR95HF_irqPulse(void)
{
	gpio_set_pin_level(IRQ_OUT, true);
	delay_ms(1);
	gpio_set_pin_level(IRQ_OUT, false);
	delay_ms(1);
	gpio_set_pin_level(IRQ_OUT, true);

	delay_ms(10);
}

static void CR95HF_resetSPI(void)
{
	delay_ms(1);

	SPI_start();
	SPI_write(CR95HF_RESET);
	SPI_end();

	delay_ms(3);

	CR95HF_irqPulse();

	delay_ms(10);
}

static uint8_t CR95HF_reset(void)
{
	uint8_t attempts = 0;

	CR95HF_irqPulse();
	CR95HF_resetSPI();

	do
	{
		uint8_t command = CR95HF_COMMAND_ECHO;
		CR95HF_sendReceive(&command);

		if (rx_data[CR95HF_COMMAND_OFFSET] == CR95HF_COMMAND_ECHO)
		{
			return 0;
		}

		CR95HF_resetSPI();
		attempts++;
	}
	while (attempts < 3);

	return 1;
}

static uint8_t CR95HF_validProtocol(CR95HF_Protocol protocol)
{
	switch (protocol)
	{
		case CR95HF_PROTOCOL_OFF:
		case CR95HF_PROTOCOL_15693:
		case CR95HF_PROTOCOL_14443A:
		case CR95HF_PROTOCOL_14443B:
		case CR95HF_PROTOCOL_14443B_Anticollision:
		case CR95HF_PROTOCOL_18092:
		return 0;
		default:
		return 1;
	}
}

static uint8_t CR95HF_start(uint8_t length, CR95HF_Protocol protocol, uint8_t *parameters)
{
	uint8_t command = CR95HF_COMMAND_ECHO;

	if (CR95HF_validProtocol(protocol) != 0)
	{
		rx_data[CR95HF_COMMAND_OFFSET] = EINVALIDPROTOCOL;
		return 1;
	}

	if (CR95HF_sendReceive(&command) != 0)
	{
		return 1;
	}

	if (rx_data[CR95HF_COMMAND_OFFSET] != CR95HF_COMMAND_ECHO)
	{
		return 1;
	}

	uint8_t buffer[16] = { 0 };
	buffer[CR95HF_COMMAND_OFFSET] = CR95HF_PROTOCOL;
	buffer[CR95HF_LENGTH_OFFSET] = length;
	buffer[CR95HF_DATA_OFFSET] = (uint8_t)protocol;

	if ((length > 1) && parameters)
	{
		memcpy(&buffer[CR95HF_DATA_OFFSET + 1], parameters, length - 1);
	}

	CR95HF_sendReceive(buffer);
	cur_protocol = protocol;
	return 0;
}

static uint8_t CR95HF_stop(void)
{
	uint8_t parameters = 0x00;
	return CR95HF_start(CR95HF_OFF_SELECT_LEN, CR95HF_PROTOCOL_OFF, &parameters);
}

static uint8_t CR95HF_ISO15693(void)
{
	// 26Kbps, 100% modulation, wait for SOF, single subcarrier, append CRC
	uint8_t parameters = CR95HF_ISO15693_CRC | CR95HF_ISO15693_SOF | CR95HF_ISO15693_26K;
	// Demodulation gain parameters
	uint8_t demodulation[] = { CR95HF_ISO15693_GAIN, CR95HF_ISO15693_ARCB };

	// Select protocol
	if (CR95HF_start(CR95HF_ISO15693_SELECT_LEN, CR95HF_PROTOCOL_15693, &parameters) != 0)
	{
		return 1;
	}

	// Write demodulation parameters
	if (CR95HF_writeRegister(CR95HF_ARCB_LEN, CR95HF_REGISTER_ARCB, CR95HF_FLAG_INC_ADDR, demodulation) != 0)
	{
		return 1;
	}

	return 0;
}

static uint8_t CR95HF_ISO14443A(void)
{
	// Init structure
	memset(iso14443a_card.ATQA, 0x00, ISO14443A_ATQA_SIZE);
	memset(iso14443a_card.UID, 0x00, ISO14443A_MAX_UID_SIZE);
	iso14443a_card.CascadeLevel = 0x00;
	iso14443a_card.UIDSize = 0x00;
	iso14443a_card.ATSSupported = 0x00;
	iso14443a_card.IsDetected = 0x00;

	// 106Kbps txrx, PP:MM:DD = 00:00:00
	uint8_t parameters[] = { 0x00, 0x00, 0x00, 0x00, 0x02, 0x02 };
	uint8_t parameter_len;

	if (ic_version < CR95HF_VERSION_QJE)
	{
		parameter_len = 5;
	}
	else
	{
		parameter_len = 7;
	}

	// Demodulation gain parameters
	uint8_t demodulation[] = { CR95HF_ISO14443A_GAIN, CR95HF_ISO14443A_ARCB };
	// Timer Window adjustment parameters
	uint8_t timer[] = { CR95HF_REGISTER_TIMERW, CR95HF_FLAG_TIMERW_UPDATE };

	// Select protocol
	if (CR95HF_start(parameter_len, CR95HF_PROTOCOL_14443A, parameters) != 0)
	{
		return 1;
	}

	// Timer window configuration
	if (CR95HF_writeRegister(CR95HF_TIMERW_LEN, CR95HF_REGISTER_TIMERW, 0x00, timer) != 0)
	{
		return 1;
	}

	// Demodulation configuration
	if (CR95HF_writeRegister(CR95HF_ARCB_LEN, CR95HF_REGISTER_ARCB, CR95HF_FLAG_INC_ADDR, demodulation) != 0)
	{
		return 1;
	}

	delay_ms(5);
	return 0;
}

static uint8_t CR95HF_ISO14443B(void)
{
	// Initilize structure
	memset(iso14443b_card.ATQB, 0x00, ISO14443B_ATQB_SIZE);
	memset(iso14443b_card.PUPI, 0x00, ISO14443B_MAX_PUPI_SIZE);
	memset(iso14443b_card.ApplicationField, 0x00, ISO14443B_MAX_APPLI_SIZE);
	memset(iso14443b_card.ProtocolInfo, 0x00, ISO14443B_MAX_PROTOCOL_SIZE);
	iso14443b_card.IsDetected = 0;

	// 106Kbps txrx, CRC
	uint8_t parameters[] = { CR95HF_ISO14443B_T106K | CR95HF_ISO14443B_R106K | CR95HF_ISO14443B_CRC, 0x00, 0x00 };
	// Demodulation gain parameters
	uint8_t demodulation[] = { CR95HF_ISO14443B_GAIN, CR95HF_ISO14443B_ARCB };

	if (ic_version < CR95HF_VERSION_QJE)
	{
		// FDT
		parameters[1] = 0x02;
	}
	else
	{
		// PP:MM bytes
		parameters[2] = 0x1A;
	}

	// Select protocol
	if (CR95HF_start(CR95HF_ISO14443B_SELECT_LEN, CR95HF_PROTOCOL_14443B, parameters) != 0)
	{
		return 1;
	}

	// Demodulation configuration
	if (CR95HF_writeRegister(CR95HF_ARCB_LEN, CR95HF_REGISTER_ARCB, CR95HF_FLAG_INC_ADDR, demodulation) != 0)
	{
		return 1;
	}

	delay_us(5100);
	return 0;
}

static uint8_t CR95HF_ISO14443BAnticollision(void)
{
	uint8_t parameters[] = { CR95HF_ISO14443B_T106K | CR95HF_ISO14443B_R106K | CR95HF_ISO14443B_CRC, 0x04, 0x3E };

	if (CR95HF_start(CR95HF_ISO14443B_SELECT_LEN, CR95HF_PROTOCOL_14443B_Anticollision, parameters) != 0)
	{
		return 1;
	}

	return 0;
}

static uint8_t CR95HF_ISO18092(void)
{
	// Initialize structures
	memset(iso18092_card.ATQC, 0x00, ISO18092_ATQC_SIZE);
	memset(iso18092_card.UID, 0x00, ISO18092_UID_SIZE);
	iso18092_card.IsDetected = 0;

	// 212Kbps txrx, Append CRC, FWT in PP:MM = 0x01:0x0D
	uint8_t parameters[] = { CR95HF_ISO18092_T212K | CR95HF_ISO18092_R212K | CR95HF_ISO18092_CRC, CR95HF_ISO18092_FWT | 0x03, 0x01, 0x0D };
	// Demodulation gain parameters
	uint8_t demodulation[] = { CR95HF_ISO18092_GAIN, CR95HF_ISO18092_ARCB };
	// Autodetect filter parameters
	uint8_t autodetect[] = { CR95HF_ISO18092_AUTODETECTA, CR95HF_ISO18092_AUTODETECTB };

	// Protocol select
	if (CR95HF_start(CR95HF_ISO18092_SELECT_LEN, CR95HF_PROTOCOL_18092, parameters) != 0)
	{
		return 1;
	}

	// Demodulation configuration
	if (CR95HF_writeRegister(CR95HF_ARCB_LEN, CR95HF_REGISTER_ARCB, CR95HF_FLAG_INC_ADDR, demodulation) != 0)
	{
		return 1;
	}

	// Autodetect filter configuration
	if (CR95HF_writeRegister(CR95HF_AUTODETECT_LEN, CR95HF_REGISTER_AUTODETECT, CR95HF_FLAG_INC_ADDR, autodetect) != 0)
	{
		return 1;
	}

	delay_ms(20);
	delay_us(400);
	return 0;
}

uint8_t CR95HF_init(void)
{
	ext_irq_register(IRQ_IN, CR95HF_extint);

	if (CR95HF_reset() != 0)
	{
		return 1;
	}

	const char *id;
	if (CR95HF_id(&id) != 0)
	{
		return 1;
	}

	ic_version = (CR95HF_Version)rx_data[CR95HF_ROM_CODE_OFFSET];
	return 0;
}

uint8_t CR95HF_id(const char **id)
{
	uint8_t command[] = { CR95HF_COMMAND_IDN, 0x00 };

	if (CR95HF_sendReceive(command) != 0)
	{
		return 1;
	}

	*id = (const char *)&rx_data[CR95HF_DATA_OFFSET];
	return 0;
}

uint8_t CR95HF_setProtocol(CR95HF_Protocol protocol)
{
	switch (protocol)
	{
		case CR95HF_PROTOCOL_OFF:
		return CR95HF_stop();
		case CR95HF_PROTOCOL_15693:
		return CR95HF_ISO15693();
		case CR95HF_PROTOCOL_14443A:
		return CR95HF_ISO14443A();
		case CR95HF_PROTOCOL_14443B:
		return CR95HF_ISO14443B();
		case CR95HF_PROTOCOL_14443B_Anticollision:
		return CR95HF_ISO14443BAnticollision();
		case CR95HF_PROTOCOL_18092:
		return CR95HF_ISO18092();
		default:
		rx_data[CR95HF_COMMAND_OFFSET] = EINVALIDPROTOCOL;
		return 1;
	}
}

CR95HF_Card CR95HF_tagHunter(void)
{
	// NFC Type 1
	CR95HF_stop();
	delay_ms(5);
	CR95HF_ISO14443A();

	if (ISO14443A_detect(&iso14443a_card) == 0)
	{
		if (ISO14443A_topaz() == 0)
		{
			return CR95HF_CARD_NFC1;
		}
	}

	// NFC Type 2 and 4A
	CR95HF_stop();
	delay_ms(5);
	CR95HF_ISO14443A();

	if (ISO14443A_detect(&iso14443a_card) == 0)
	{
		if (ISO14443A_anticollision(&iso14443a_card) != 0)
		{
			if ((iso14443a_card.SAK & 0x60) == 0x00)
			{
				return CR95HF_CARD_NFC2;
			}
			else if ((iso14443a_card.SAK & 0x20) != 0x00)
			{
				return CR95HF_CARD_NFC4A;
			}
		}
	}

	// NFC Type 3
	CR95HF_stop();
	delay_ms(5);
	CR95HF_ISO18092();

	if (ISO18092_detect(&iso18092_card) == 0)
	{
		return CR95HF_CARD_NFC3;
	}

	// NFC Type 4B
	CR95HF_stop();
	delay_ms(5);
	CR95HF_ISO14443B();

	if (ISO14443B_detect(&iso14443b_card) == 0)
	{
		if (ISO14443B_anticollision(&iso14443b_card) == 0)
		{
			return CR95HF_CARD_NFC4B;
		}
	}

	// NFC Type 5
	CR95HF_stop();
	delay_ms(5);
	CR95HF_ISO15693();

	if (ISO15693_detect() == 0)
	{
		return CR95HF_CARD_NFC5;
	}

	CR95HF_stop();
	return CR95HF_CARD_NONE;
}

const uint8_t* CR95HF_data(void)
{
	return (const uint8_t*)rx_data;
}

uint8_t CR95HF_checkCRC(void)
{
	uint8_t length = rx_data[CR95HF_LENGTH_OFFSET];

	switch (cur_protocol)
	{
		case CR95HF_PROTOCOL_14443A:
		if ((rx_data[CR95HF_DATA_OFFSET + length - ISO14443A_NBBYTE] & ISO14443A_CRCMASK) != ISO14443A_CRCMASK)
		{
			return 0;
		}

		break;
		case CR95HF_PROTOCOL_14443B:
		case CR95HF_PROTOCOL_14443B_Anticollision:
		if ((rx_data[CR95HF_DATA_OFFSET + length - ISO14443B_NBBYTE] & ISO14443B_CRCMASK) != ISO14443B_CRCMASK)
		{
			return 0;
		}

		break;
		case CR95HF_PROTOCOL_18092:
		if ((rx_data[CR95HF_DATA_OFFSET + length - ISO18092_NBBYTE] & ISO18092_CRCMASK) != ISO18092_CRCMASK)
		{
			return 0;
		}

		break;
		case CR95HF_PROTOCOL_15693:
		if ((rx_data[CR95HF_DATA_OFFSET + length - ISO15693_NBBYTE] & ISO15693_CRCMASK) != ISO15693_CRCMASK)
		{
			return 0;
		}

		break;
		default:
		return 1;
	}

	return 1;
}