#include <stdint.h>
#include <string.h>

#include "../CR95HF.h"
#include "iso18092.h"

uint8_t ISO18092_detect(ISO18092_Card *card)
{
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x05, 0x00, 0x12, 0xFC, 0x01, 0x03 };

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();

	if (data[CR95HF_DATA_OFFSET] != 0x01)
	{
		return 1;
	}

	card->IsDetected = 1;
	memcpy(card->ATQC, &data[CR95HF_DATA_OFFSET], ISO18092_ATQC_SIZE);
	memcpy(card->UID, &card->ATQC[1], ISO18092_UID_SIZE);

	return 0;
}
