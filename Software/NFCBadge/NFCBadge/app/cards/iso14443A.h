#ifndef APP_ISO14443A_H
#define APP_ISO14443A_H

#include <stdint.h>

// status and error code
#define ISO14443A_SUCCESSCODE			0x00
#define ISO14443A_ERRORCODE_DEFAULT		0x61
#define ISO14443A_ERRORCODE_CRC			0x62

// Anticollison levels (commands)
#define ISO14443A_SEL_CASCADE_LVL_1		0x93
#define ISO14443A_SEL_CASCADE_LVL_2		0x95
#define ISO14443A_SEL_CASCADE_LVL_3		0x97
#define ISO14443A_COMMAND_REQA			0x26
#define ISO14443A_COMMAND_WUPA			0x52
#define ISO14443A_COMMAND_SELECT_LV1	0x93
#define ISO14443A_COMMAND_SELECT_LV2	0x95
#define ISO14443A_COMMAND_SELECT_LV3	0x97
#define ISO14443A_COMMAND_HLTA			0x50
#define ISO14443A_COMMAND_RATS			0xE0
#define ISO14443A_COMMAND_PPS			0xD0
#define ISO14443A_COMMAND_DESELECT		0xC2
#define ISO14443A_COMMAND_DESELECTCID	0xCA

// Iblock
#define ISO14443A_COMMAND_IBLOCK02		0x02
#define ISO14443A_COMMAND_IBLOCK03		0x03
#define ISO14443A_COMMAND_SBLOCK		0xC2
#define ISO14443A_COMMAND_NACKBLOCK_B2	0xB2
#define ISO14443A_COMMAND_NACKBLOCK_B3	0xB3
#define ISO14443A_COMMAND_ACKBLOCK		0xA2


// numbr of the cascade level
#define ISO14443A_CASCADE_LVL_1			1
#define ISO14443A_CASCADE_LVL_2			2
#define ISO14443A_CASCADE_LVL_3			3

#define ISO14443A_NVM_10				0x10
#define ISO14443A_NVM_20				0x20
#define ISO14443A_NVM_30				0x30
#define ISO14443A_NVM_40				0x40
#define ISO14443A_NVM_50				0x50
#define ISO14443A_NVM_60				0x60
#define ISO14443A_NVM_70				0x70

// UID Sizes
#define ISO14443A_UIDSIZE_UNDEFINED		-1
#define ISO14443A_UID_PART				3
#define ISO14443A_UID_SINGLE_SIZE		4
#define	ISO14443A_UID_DOUBLE_SIZE		7
#define ISO14443A_UID_TRIPLE_SIZE		10

// Mask used for ATQA
#define ISO14443A_UID_MASK				0xC0
#define ISO14443A_AC_BIT_FRAME_MASK		0x1F
#define ISO14443A_CID_MASK				0x0F
#define ISO14443A_FSDI_MASK				0xF0

// Size for ISO14443A variables
#define ISO14443A_MAX_NAME_SIZE			50
#define ISO14443A_MAX_UID_SIZE			10
#define ISO14443A_ATQA_SIZE				2
#define ISO14443A_REQA_SIZE				2

// SAK FLAG
#define ISO14443A_FLAG_ATS_SUPPORTED	0x20
#define ISO14443A_FLAG_UID_INCOMPLETE	0x04

// ATQ FLAG
#define ISO14443A_FLAG_UID_SINGLE_SIZE	0
#define	ISO14443A_FLAG_UID_DOUBLE_SIZE	1
#define ISO14443A_FLAG_UID_TRIPLE_SIZE	2

// Flags
#define ISO14443A_FLAG_ATQA				0x07
#define ISO14443A_FLAG_8BITS			0x08

// CRC
#define ISO14443A_CRCMASK				0x20
#define ISO14443A_CRC_ERR_TYPEA			0x20

#define ISO14443A_NBBYTE				0x03

typedef struct
{
	// ATQA: Answer to request of type A
	uint8_t ATQA[ISO14443A_ATQA_SIZE];
	// Cascade level: Position in anticollision
	uint8_t CascadeLevel;
	// UID: Unique card identifier
	uint8_t UID[ISO14443A_MAX_UID_SIZE];
	uint8_t UIDSize;
	// SAK: Select acknowledge
	uint8_t SAK;
	// ATS: Answer to select
	uint8_t ATSSupported;
	// Valid card detected
	uint8_t IsDetected;
	// CID: Card Identifier
	uint8_t CID;
	// FSDI: Frame Size for proximity coupling Device Integer
	uint8_t FSDI;
	// DRI: Device Receive Integer
	uint8_t DRI;
	// DSI: Device Send Integer
	uint8_t DSI;
} ISO14443A_Card;

uint8_t ISO14443A_detect(ISO14443A_Card *card);
uint8_t ISO14443A_topaz(void);
uint8_t ISO14443A_anticollision(ISO14443A_Card *card);

#endif
