#include <string.h>
#include <stdint.h>

#include "../CR95HF.h"
#include "iso14443A.h"

static uint8_t ISO14443A_AC(uint8_t level, uint8_t *buffer)
{
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x03, level, ISO14443A_NVM_20, 0x08, 0x00, 0x00, 0x00, 0x00 };
	uint8_t UID[4] = { 0 };

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();

	const uint8_t responseOrigin = data[CR95HF_LENGTH_OFFSET];
	uint8_t response  = responseOrigin;

	uint8_t collision = data[response - 1] & 0x80;
	uint8_t byteCollision = data[response];
	uint8_t bitCollision = data[response + 1];

	if (bitCollision == 8)
	{
		return 1;
	}

	// Check for collision
	while (collision == 0x80)
	{
		// Clear collision status
		collision = 0;

		// Build anticollision message
		parameters[CR95HF_DATA_OFFSET + 1] = ISO14443A_NVM_20 + ((byteCollision << 4) + (bitCollision + 1));

		if (byteCollision == 0)
		{
			parameters[CR95HF_DATA_OFFSET + 2] = data[CR95HF_DATA_OFFSET] & ~(0xFF << (bitCollision + 1));
			parameters[CR95HF_DATA_OFFSET + 3] = (bitCollision + 1) | 0x40;

			UID[0] = parameters[CR95HF_DATA_OFFSET + 2];
		}
		else if (byteCollision == 1)
		{
			parameters[CR95HF_DATA_OFFSET + 2] = data[CR95HF_DATA_OFFSET];
			parameters[CR95HF_DATA_OFFSET + 3] = data[CR95HF_DATA_OFFSET + 1] & ~(0xFF << (bitCollision + 1));
			parameters[CR95HF_DATA_OFFSET + 4] = (bitCollision + 1) | 0x40;

			memcpy(UID, &parameters[CR95HF_DATA_OFFSET + 2], 2);
		}
		else if (byteCollision == 2)
		{
			memcpy(&parameters[CR95HF_DATA_OFFSET + 2], &data[CR95HF_DATA_OFFSET], 2);

			parameters[CR95HF_DATA_OFFSET + 4] = data[CR95HF_DATA_OFFSET + 2] & ~(0xFF << (bitCollision + 1));
			parameters[CR95HF_DATA_OFFSET + 5] = (bitCollision + 1) | 0x40;

			memcpy(UID, &parameters[CR95HF_DATA_OFFSET + 2], 3);
		}
		else if (byteCollision == 3)
		{
			memcpy(&parameters[CR95HF_DATA_OFFSET + 2], &data[CR95HF_DATA_OFFSET], 3);

			parameters[CR95HF_DATA_OFFSET + 5] = data[CR95HF_DATA_OFFSET + 3] & ~(0xFF << (bitCollision + 1));
			parameters[CR95HF_DATA_OFFSET + 6] = (bitCollision + 1) | 0x40;

			memcpy(UID, &parameters[CR95HF_DATA_OFFSET + 2], 4);
		}
		else
		{
			return 1;
		}

		if (CR95HF_sendReceive(parameters) != 0)
		{
			return 1;
		}

		if (data[CR95HF_COMMAND_OFFSET] != 0x80)
		{
			return 1;
		}

		// Check to see if there is another collision
		response = data[CR95HF_LENGTH_OFFSET];
		collision = data[response - 1] & 0x80;

		uint8_t newByteCollision = 0;
		uint8_t newBitCollision = 0;

		if (collision == 0x80)
		{
			newBitCollision = data[response];
			newBitCollision = data[response + 1];
		}

		// Check alignment
		if ((bitCollision + 1) == (8 - (0x0F & data[2 + (response - 2) - 1])))
		{
			// Prepare UID
			if (byteCollision == 0)
			{
				UID[0] = (~(0xFF << (bitCollision + 1)) & parameters[CR95HF_DATA_OFFSET + 2]) | data[CR95HF_DATA_OFFSET];
				memcpy(&UID[1], &data[CR95HF_DATA_OFFSET + 1], 3);
			}
			else if (byteCollision == 1)
			{
				UID[1] = (~(0xFF << (bitCollision + 1)) & parameters[CR95HF_DATA_OFFSET + 3]) | data[CR95HF_DATA_OFFSET];
				memcpy(&UID[2], &data[CR95HF_DATA_OFFSET + 1], 2);
			}
			else if (byteCollision == 2)
			{
				UID[2] = (~(0xFF << (bitCollision + 1)) & parameters[CR95HF_DATA_OFFSET + 4]) | data[CR95HF_DATA_OFFSET];
				UID[3] = data[CR95HF_DATA_OFFSET + 1];
			}
			else if (byteCollision == 3)
			{
				UID[3] = (~(0xFF << (bitCollision + 1)) & parameters[CR95HF_DATA_OFFSET + 5]) | data[CR95HF_DATA_OFFSET];
			}
			else
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}

		// Prepare buffer for caller
		if (buffer)
		{
			buffer[0] = 0x80;
			buffer[1] = responseOrigin;
			memcpy(&buffer[2], UID, 4);
			buffer[6] = UID[0] ^ UID[1] ^ UID[2] ^ UID[3];
		}

		// Setup collision indexes for next collision
		if (collision == 0x80)
		{
			if (byteCollision != newByteCollision)
			{
				byteCollision += newByteCollision;
				bitCollision = newBitCollision;
			}
			else
			{
				byteCollision += newByteCollision;
				bitCollision += newBitCollision + 1;
			}
		}
	}

	return 0;
}

static uint8_t ISO14443A_ACL1(ISO14443A_Card *card)
{
	uint8_t buffer[7] = { 0 };

	if (ISO14443A_AC(ISO14443A_SEL_CASCADE_LVL_1, buffer) != 0)
	{
		return 1;
	}

	if (card->UIDSize == ISO14443A_UID_SINGLE_SIZE)
	{
		memcpy(card->UID, &buffer[CR95HF_DATA_OFFSET], ISO14443A_UID_SINGLE_SIZE);
	}
	else
	{
		memcpy(card->UID, &buffer[CR95HF_DATA_OFFSET + 1], ISO14443A_UID_PART);
	}

	const uint8_t bcc = buffer[CR95HF_DATA_OFFSET + ISO14443A_UID_SINGLE_SIZE];

	// Prepare select command
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x08, ISO14443A_SEL_CASCADE_LVL_1, ISO14443A_NVM_70, 0x00, 0x00, 0x00, 0x00, bcc, ISO14443A_CRCMASK | ISO14443A_FLAG_8BITS };

	if (card->UIDSize == ISO14443A_UID_SINGLE_SIZE)
	{
		memcpy(&parameters[CR95HF_DATA_OFFSET + 2], card->UID, ISO14443A_UID_SINGLE_SIZE);
	}
	else
	{
		parameters[CR95HF_DATA_OFFSET + 2] = 0x88;
		memcpy(&parameters[CR95HF_DATA_OFFSET + 3], card->UID, ISO14443A_UID_PART);
	}

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();
	card->SAK = data[CR95HF_DATA_OFFSET];
	return 0;
}

static uint8_t ISO14443A_ACL2(ISO14443A_Card *card)
{
	uint8_t buffer[7] = { 0 };

	if (ISO14443A_AC(ISO14443A_SEL_CASCADE_LVL_2, buffer) != 0)
	{
		return 1;
	}

	if (card->UIDSize == ISO14443A_UID_DOUBLE_SIZE)
	{
		memcpy(&card->UID[ISO14443A_UID_PART], &buffer[CR95HF_DATA_OFFSET], ISO14443A_UID_SINGLE_SIZE);
	}
	else
	{
		memcpy(&card->UID[ISO14443A_UID_PART], &buffer[CR95HF_DATA_OFFSET + 1], ISO14443A_UID_PART);
	}

	const uint8_t bcc = buffer[CR95HF_DATA_OFFSET + ISO14443A_UID_SINGLE_SIZE];

	// Prepare select command
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x08, ISO14443A_SEL_CASCADE_LVL_2, ISO14443A_NVM_70, 0x00, 0x00, 0x00, 0x00, bcc, ISO14443A_CRCMASK | ISO14443A_FLAG_8BITS };

	if (card->UIDSize == ISO14443A_UID_DOUBLE_SIZE)
	{
		memcpy(&parameters[CR95HF_DATA_OFFSET + 2], &card->UID[ISO14443A_UID_PART], ISO14443A_UID_SINGLE_SIZE);
	}
	else
	{
		parameters[CR95HF_DATA_OFFSET + 2] = 0x88;
		memcpy(&parameters[CR95HF_DATA_OFFSET + 3], &card->UID[ISO14443A_UID_PART], ISO14443A_UID_PART);
	}

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();
	card->SAK = data[CR95HF_DATA_OFFSET];
	return 0;
}

static uint8_t ISO14443A_ACL3(ISO14443A_Card *card)
{
	uint8_t buffer[7] = { 0 };

	if (ISO14443A_AC(ISO14443A_SEL_CASCADE_LVL_3, buffer) != 0)
	{
		return 1;
	}

	memcpy(&card->UID[ISO14443A_UID_PART], &buffer[CR95HF_DATA_OFFSET], ISO14443A_UID_SINGLE_SIZE);

	const uint8_t bcc = buffer[CR95HF_DATA_OFFSET + ISO14443A_UID_SINGLE_SIZE];

	// Prepare select command
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x04, ISO14443A_SEL_CASCADE_LVL_3, ISO14443A_NVM_70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	if (card->UIDSize == ISO14443A_UID_TRIPLE_SIZE)
	{
		memcpy(&parameters[CR95HF_DATA_OFFSET + 2], &card->UID[ISO14443A_UID_PART], ISO14443A_UID_SINGLE_SIZE);

		parameters[CR95HF_LENGTH_OFFSET] += ISO14443A_UID_SINGLE_SIZE;
		parameters[CR95HF_DATA_OFFSET + 2 + ISO14443A_UID_SINGLE_SIZE] = bcc;
		parameters[CR95HF_DATA_OFFSET + 3 + ISO14443A_UID_SINGLE_SIZE] = ISO14443A_CRCMASK | ISO14443A_FLAG_8BITS;
	}
	else
	{
		parameters[CR95HF_DATA_OFFSET + 2] = bcc;
		parameters[CR95HF_DATA_OFFSET + 3] = ISO14443A_CRCMASK | ISO14443A_FLAG_8BITS;
	}

	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();
	card->SAK = data[CR95HF_DATA_OFFSET];
	return 0;
}

static uint8_t ISO14443A_HALT(void)
{
	uint8_t parameters[] = { CR95HF_COMMAND_SENDRECV, 0x03, 0x50, 0x00, 0x28 };

	// HALT card
	if (CR95HF_sendReceive(parameters) != 0)
	{
		return 1;
	}

	return 0;
}

uint8_t ISO14443A_detect(ISO14443A_Card *card)
{
	uint8_t request[] = { CR95HF_COMMAND_SENDRECV, ISO14443A_REQA_SIZE, ISO14443A_COMMAND_REQA, ISO14443A_FLAG_ATQA };

	if (CR95HF_sendReceive(request) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();

	// Copy ATQ response
	memcpy(card->ATQA, &data[CR95HF_DATA_OFFSET], ISO14443A_ATQA_SIZE);

	// Recover UID size
	switch ((card->ATQA[0] & ISO14443A_UID_MASK) >> 6)
	{
		case ISO14443A_FLAG_UID_SINGLE_SIZE:
			card->UIDSize = ISO14443A_UID_SINGLE_SIZE;
			break;
		case ISO14443A_FLAG_UID_DOUBLE_SIZE:
			card->UIDSize = ISO14443A_UID_DOUBLE_SIZE;
			break;
		case ISO14443A_FLAG_UID_TRIPLE_SIZE:
			card->UIDSize = ISO14443A_UID_TRIPLE_SIZE;
			break;
	}

	card->IsDetected = 1;
	return 0;
}

uint8_t ISO14443A_topaz(void)
{
	uint8_t request[] = { CR95HF_COMMAND_SENDRECV, 0x08, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA8 };

	if (CR95HF_sendReceive(request) != 0)
	{
		return 1;
	}

	const uint8_t *data = CR95HF_data();
	const uint8_t length = data[CR95HF_LENGTH_OFFSET];

	if ((data[length - 1] & ISO14443A_CRCMASK) == ISO14443A_CRC_ERR_TYPEA)
	{
		return 1;
	}

	const uint8_t command = data[CR95HF_COMMAND_OFFSET];

	if (command == 0x80)
	{
		if (length == 0x04)
		{
			return 1;
		}

		return 0;
	}

	return 1;
}

uint8_t ISO14443A_anticollision(ISO14443A_Card *card)
{
	if (ISO14443A_ACL1(card) != 0)
	{
		return 1;
	}

	// Continue reading UID if necessary
	if (card->SAK & ISO14443A_FLAG_UID_INCOMPLETE)
	{
		if (ISO14443A_ACL2(card) != 0)
		{
			return 1;
		}
	}

	// Finish reading UID if necessary
	if (card->SAK & ISO14443A_FLAG_UID_INCOMPLETE)
	{
		if (ISO14443A_ACL3(card) != 0)
		{
			return 1;
		}
	}

	// Card found, halt it
	if (ISO14443A_HALT() != 0)
	{
		return 1;
	}

	return 0;
}
