#include <stdint.h>
#include <string.h>

#include "../CR95HF.h"
#include "iso15693.h"

uint8_t ISO15693_detect(void)
{
	uint8_t uid[ISO15693_NBBYTE_UID];

	if (ISO15693_getUID(uid) == 0)
	{
		return 0;
	}

	return 1;
}

uint8_t ISO15693_getUID(uint8_t *uid)
{
	// Single subcarrier, 52kb/s txrx, inventory set, no extensions, no AFI, 1 slot, no options, no RFU
	const uint8_t flags = 0x64;

	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x03, flags, ISO15693_CMDCODE_INVENTORY, 0x00 };

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();
	uint8_t error = data[CR95HF_LENGTH_OFFSET] + 1;

	if ((data[error] & ISO15693_CRC_MASK) != 0x00)
	{
		return 1;
	}

	memcpy(uid, &data[ISO15693_OFFSET_UID], ISO15693_NBBYTE_UID);
	return 0;
}