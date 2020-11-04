#ifndef APP_CR95HF_H
#define APP_CR95HF_H

#define CR95HF_RESET							0x01
#define CR95HF_PROTOCOL							0x02

// RFtransceiver HEADER command definition
#define CR95HF_COMMAND_SEND						0x00
#define CR95HF_COMMAND_IDN						0x01
#define CR95HF_COMMAND_RECEIVE					0x02
#define CR95HF_COMMAND_POLLING					0x03
#define CR95HF_COMMAND_SENDRECV					0x04
#define CR95HF_COMMAND_IDLE						0x07
#define CR95HF_COMMAND_RDREG					0x08
#define CR95HF_COMMAND_WRREG					0x09
#define CR95HF_COMMAND_ECHO						0x55

// RF transceiver status
#define CR95HF_SUCCESS_CODE						0x01
#define CR95HF_NOREPLY_CODE						0x01
#define	CR95HF_ERRORCODE_DEFAULT				0xFE
#define	CR95HF_ERRORCODE_TIMEOUT				0xFD
#define CR95HF_ERRORCODE_POR					0x44

// RF transceiver polling status
#define CR95HF_POLLING							0x00
#define CR95HF_POLLING_TIMEOUT					0x01

// RF transceiver number of byte of the buffers
#define CR95HF_RESPONSEBUFFER_SIZE				0xFF
#define CR95HF_MAX_BUFFER_SIZE					0xFF

// Max of 256 (for QJC) and 768 (for QJE. Should be 512 but bug Design)
#define CR95HF_MAX_ECHO							768

// RF transceiver Offset of the command and the response
#define CR95HF_COMMAND_OFFSET					0x00
#define CR95HF_LENGTH_OFFSET					0x01
#define CR95HF_DATA_OFFSET						0x02

// ECHO response
#define CR95HF_ECHORESPONSE						0x55

// Sleep parameters
#define CR95HF_IDLE_SLEEP_MODE					0x00
#define CR95HF_IDLE_HIBERNATE_MODE				0x01
#define CR95HF_IDLE_CMD_LENTH					0x0E

#define CR95HF_WU_TIMEOUT						0x01
#define CR95HF_WU_TAG							0x02
#define CR95HF_WU_FIELD							0x04
#define CR95HF_WU_IRQ							0x08
#define CR95HF_WU_SPI							0x10
#define CR95HF_HIBERNATE_ENTER_CTRL				0x0400
#define CR95HF_SLEEP_ENTER_CTRL					0x0100
#define CR95HF_SLEEP_FIELD_ENTER_CTRL			0x0142
#define CR95HF_HIBERNATE_WU_CTRL				0x0400
#define CR95HF_SLEEP_WU_CTRL					0x3800
#define CR95HF_LEAVE_CTRL						0x1800

// Calibration parameters
#define CR95HF_WU_SOURCE_OFFSET					0x02
#define CR95HF_WU_PERIOD_OFFSET					0x09
#define CR95HF_DACDATAL_OFFSET					0x0C
#define CR95HF_DACDATAH_OFFSET					0x0D
#define CR95HF_NBTRIALS_OFFSET					0x0F
#define CR95HF_ROM_CODE_OFFSET					13

#define CR95HF_LISTEN							0x05

#define CR95HF_READERREPLY_MAX_BUFFER_SIZE		0x40

#define CR95HF_ARCB_LEN							0x04
#define CR95HF_TIMERW_LEN						0x04
#define CR95HF_AUTODETECT_LEN					0x04

// Registers
#define CR95HF_REGISTER_AUTODETECT				0x0A
#define CR95HF_REGISTER_TIMERW					0x5A
#define CR95HF_REGISTER_ARCB					0x68

// Flags
#define CR95HF_FLAG_INC_ADDR					0x01
#define CR95HF_FLAG_TIMERW_UPDATE				0x04
#define CR95HF_FLAG_DATA_READY					0x08
#define CR95HF_FLAG_DATA_READY_MASK				0x08

// Errors
#define EEMDSOFERROR23 							0x63 // SOF error in high part (duration 2 to 3 etu) in ISO/IEC 14443B
#define EEMDSOFERROR10 							0x65 // SOF error in low part (duration 10 to 11 etu) in ISO/IEC 14443B
#define EEMDEGT 								0x66 // error Extennded Guard Time error in ISO/IEC 14443B
#define ETR1 									0x67 // Too Big Too long TR1 send by the card, reception stopped in ISO/IEC 14443BT
#define ETR1TOO 								0x68 // small Too small TR1 send by the card in ISO/IEC 14443B
#define EINTERNALERROR 							0x71 // Wrong frame format decodes
#define EFRAMERECVOK 							0x80 // Frame correctly received (additionally see CRC/Parity information)
#define EUSERSTOP 								0x85 // Stopped by user (used only in Card mode)
#define ECOMMERROR								0x86 // Hardware communication error
#define EFRAMEWAITTOUT 							0x87 // Frame wait time out (no valid reception)
#define EINVALIDSOF 							0x88 // Invalid SOF
#define EBUFOVERFLOW 							0x89 // Too many bytes received and data still arriving
#define EFRAMINGERROR 							0x8A // if start bit = 1 or stop bit = 0
#define EEGTERROR 								0x8B // EGT time out
#define EINVALIDLEN 							0x8C // Valid for ISO/IEC 18092, if Length <3
#define ECRCERROR 								0x8D // CRC error, Valid only for ISO/IEC 18092
#define ERECVLOST 								0x8E // When reception is lost without EOF received (or subcarrier was lost)
#define ENOFIELD 								0x8F // When Listen command detects the absence of external field
#define EUNINTBYTE 								0x90 // Residual bits in last byte. Us

// Application errors
#define EINVALIDPROTOCOL						0xF0 // Invalid protocol

// OFF
#define CR95HF_OFF_SELECT_LEN					0x02

// ISO15693
#define CR95HF_ISO15693_CRC						(0x01 << 0) // Append CRC
#define CR95HF_ISO15693_DSC						(0x01 << 1) // Demodulation dual subcarrier
#define CR95HF_ISO15693_MOD						(0x01 << 2) // 10% modulation
#define CR95HF_ISO15693_SOF						(0x01 << 3) // Wait for SOF
#define CR95HF_ISO15693_26K						(0x00 << 4) // 26Kbps transfer rate
#define CR95HF_ISO15693_52K						(0x01 << 4) // 52Kbps transfer rate
#define CR95HF_ISO15693_6K						(0x02 << 4) // 6Kbps transfer rate
#define CR95HF_ISO15693_SELECT_LEN				0x02
#define CR95HF_ISO15693_GAIN					0x01
#define CR95HF_ISO15693_ARCB					0xD1

// ISO14443A
#define CR95HF_ISO14443A_T106K					(0x00 << 6) // Transmit 106Kbps
#define CR95HF_ISO14443A_T212K					(0x01 << 6) // Transmit 212Kbps
#define CR95HF_ISO14443A_T424K					(0x02 << 6) // Transmit 424Kbps
#define CR95HF_ISO14443A_R106K					(0x00 << 4) // Receive 106Kbps
#define CR95HF_ISO14443A_R212K					(0x01 << 4) // Receive 212Kbps
#define CR95HF_ISO14443A_R424K					(0x02 << 4) // Receive 424Kbps
#define CR95HF_ISO14443A_GAIN					0x01
#define CR95HF_ISO14443A_ARCB					0xDF

// ISO14443B
#define CR95HF_ISO14443B_T106K					(0x00 << 6) // Transmit 106Kbps
#define CR95HF_ISO14443B_T212K					(0x01 << 6) // Transmit 212Kbps
#define CR95HF_ISO14443B_T424K					(0x02 << 6) // Transmit 424Kbps
#define CR95HF_ISO14443B_T848K					(0x03 << 6) // Transmit 848Kbps
#define CR95HF_ISO14443B_R106K					(0x00 << 4) // Receive 106Kbps
#define CR95HF_ISO14443B_R212K					(0x01 << 4) // Receive 212Kbps
#define CR95HF_ISO14443B_R424K					(0x02 << 4) // Receive 424Kbps
#define CR95HF_ISO14443B_R848K					(0x03 << 4) // Receive 848Kbps
#define CR95HF_ISO14443B_CRC					(0x01 << 0) // Append CRC
#define CR95HF_ISO14443B_SELECT_LEN				4
#define CR95HF_ISO14443B_GAIN					0x01
#define CR95HF_ISO14443B_ARCB					0x51

// ISO18092
#define CR95HF_ISO18092_T212K					(0x01 << 6) // Transmit 212Kbps
#define CR95HF_ISO18092_T424K					(0x02 << 6) // Transmit 424Kbps
#define CR95HF_ISO18092_R212K					(0x01 << 4) // Receive 212Kbps
#define CR95HF_ISO18092_R424K					(0x02 << 4) // Receive 424Kbps
#define CR95HF_ISO18092_CRC						(0x01 << 0) // Append CRC
#define CR95HF_ISO18092_FWT						(0x01 << 4) // Frame Waiting Time specified by PP:MM
#define CR95HF_ISO18092_SLOT_MSK				(0x0F << 0) // Bit mask for slots
#define CR95HF_ISO18092_SELECT_LEN				5
#define CR95HF_ISO18092_GAIN					0x01
#define CR95HF_ISO18092_ARCB					0x51
#define CR95HF_ISO18092_AUTODETECTA				0x02
#define CR95HF_ISO18092_AUTODETECTB				0xA1

typedef enum
{
	CR95HF_PROTOCOL_OFF = 0x00,
	CR95HF_PROTOCOL_15693 = 0x01,
	CR95HF_PROTOCOL_14443A = 0x02,
	CR95HF_PROTOCOL_14443B = 0x03,
	CR95HF_PROTOCOL_14443B_Anticollision = 0x04,
	CR95HF_PROTOCOL_18092 = 0x05
} CR95HF_Protocol;

typedef enum
{
	CR95HF_VERSION_QJA = 0x30, // '0'
	CR95HF_VERSION_QJB = 0x31, // '1'
	CR95HF_VERSION_QJC = 0x32, // '2'
	CR95HF_VERSION_QJD = 0x33, // '3'
	CR95HF_VERSION_QJE = 0x34  // '4'
} CR95HF_Version;

typedef enum
{
	CR95HF_CARD_NONE = 0,
	CR95HF_CARD_NFC1 = 1,
	CR95HF_CARD_NFC2 = 2,
	CR95HF_CARD_NFC3 = 3,
	CR95HF_CARD_NFC4A = 4,
	CR95HF_CARD_NFC4B = 5,
	CR95HF_CARD_NFC5 = 6
} CR95HF_Card;

uint8_t CR95HF_init(void);
uint8_t CR95HF_id(const char **id);
uint8_t CR95HF_setProtocol(CR95HF_Protocol protocol);
CR95HF_Card CR95HF_tagHunter(void);
uint8_t CR95HF_sendReceive(uint8_t *data);
const uint8_t* CR95HF_data(void);
uint8_t CR95HF_checkCRC(void);

#endif
