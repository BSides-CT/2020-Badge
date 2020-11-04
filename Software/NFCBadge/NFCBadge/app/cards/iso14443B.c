#include <stdint.h>
#include <string.h>

#include "../CR95HF.h"
#include "iso14443B.h"

uint8_t ISO14443B_detect(ISO14443B_Card *card)
{
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x03, ISO14443B_ANTICOLLISION_PREFIX_BYTE, ISO14443B_AFI_ALL_FAMILIES, ISO14443B_EXTENDED_ATQB_NOT_SUPPORTED | ISO14443B_REQB_ATTEMPT | ISO14443B_SLOT_MARKER_1 };

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();

	card->IsDetected = 1;
	memcpy(card->ATQB, &data[CR95HF_DATA_OFFSET], ISO14443B_ATQB_SIZE);
	memcpy(card->PUPI, &data[CR95HF_DATA_OFFSET + 1], ISO14443B_MAX_PUPI_SIZE);
	memcpy(card->ApplicationField, &data[CR95HF_DATA_OFFSET + 1 + ISO14443B_MAX_PUPI_SIZE], ISO14443B_MAX_APPLI_SIZE);
	memcpy(card->ProtocolInfo, &data[CR95HF_DATA_OFFSET + 1 + ISO14443B_MAX_PUPI_SIZE + ISO14443B_MAX_APPLI_SIZE], ISO14443B_MAX_PROTOCOL_SIZE);

	return 0;
}

uint8_t ISO14443B_anticollision(ISO14443B_Card *card)
{
	uint8_t parameters[] =
	{
		CR95HF_COMMAND_SENDRECV,
		0x09,
		ATTRIB_FIRST_BYTE,
		card->PUPI[0], card->PUPI[1], card->PUPI[2], card->PUPI[3],
		TR0_64_FS | TR1_64_FS | EOF_REQUIRED | SOF_REQUIRED,
		MAX_FRAME_SIZE_256_BYTES | PCD_TO_PICC_106K | PICC_TO_PCD_106K,
		TR2_32_FS | PICC_COMPLIANT_ISO14443_4,
		CID_0
	};

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	if (CR95HF_checkCRC() != 0)
	{
		return 1;
	}

	if (CR95HF_setProtocol(CR95HF_PROTOCOL_14443B_Anticollision) != 0)
	{
		return 1;
	}

	return 0;
}
