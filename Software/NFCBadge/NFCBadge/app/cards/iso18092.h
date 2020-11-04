#ifndef APP_ISO18092_H
#define APP_ISO18092_H

#include <stdint.h>

#define ISO18092_ATQC_SIZE				0x20
#define ISO18092_UID_SIZE				8

/* command code -------------------------------------------------------------- */
#define ISO18092_COMMAND_REQC			0x00
#define ISO18092_RESPONSE_REQC			0x01

/* code status	-------------------------------------------------------------------------- */
#define ISO18092_SUCCESSCODE			0x00
#define ISO18092_ERRORCODE_DEFAULT		0xC1

#define ISO18092_NBBYTE					0x01
#define ISO18092_CRCMASK				0x02

typedef struct
{
	// ATQC: Answer to request of type C
	uint8_t ATQC[ISO18092_ATQC_SIZE];
	// UID: Unique card identifier
	uint8_t UID[ISO18092_UID_SIZE];
	// Is Detected: Valid card detection
	uint8_t IsDetected;
} ISO18092_Card;

uint8_t ISO18092_detect(ISO18092_Card *card);

#endif
