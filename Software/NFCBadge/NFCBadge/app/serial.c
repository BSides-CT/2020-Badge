#include <atmel_start.h>

#include "serial.h"
//#include "menu.h"

#define RX_LEN 256

typedef enum
{
	EBUFFULL
} serial_error;

static uint8_t rx_data[RX_LEN];
static uint32_t rx_pos = 0;
static uint8_t connected = 0;

static void serial_flush(void)
{
	rx_pos = 0;
	memset(rx_data, 0x00, RX_LEN);
}
/*
static void serial_errorMSG(serial_error error)
{
	switch (error)
	{
		case EBUFFULL:
			serial_writeString("ERROR: rx buffer full (limit 255)\r\n");
			break;
		default:
			serial_writeString("ERROR: unknown error\r\n");
			break;
	}
}

static void serial_processCommand(uint8_t size)
{
	// If length is zero (no room in buffer during read)
	if (size == 0)
	{
		// Return to the beginning and print an error
		serial_flush();
		serial_errorMSG(EBUFFULL);
		return;
	}

	for (uint16_t i = rx_pos; i <= (RX_LEN - size); i++)
	{
		// Look for CRLF
		if ((rx_data[i] == 0x0A) || (rx_data[i] == 0x0D))
		{
			rx_data[i + 1] = 0x00; // Null terminate
			menu_getEntry((const char *)&rx_data[rx_pos]);
			serial_flush();
			return;
		}
	}

	rx_pos += size;

	if (rx_pos >= (RX_LEN - 1))
	{
		serial_flush();
	}
}
*/
static uint8_t serial_read(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	// Sanity check, wrap rx_pos to prevent underflow
	if (rx_pos >= (RX_LEN - 1))
	{
		serial_flush();
	}

	uint32_t length = RX_LEN - rx_pos - 1; // Limit to 255 for null termination
	uint8_t retval = 0;

	if (count < length)
	{
		length = count;
	}
	else
	{
		// Ran out of data in the buffer, stall the endpoint
		retval = 1;
	}

	if (length != 0)
	{
		// Respond to bulk in request
		cdcdf_acm_read(&rx_data[rx_pos], length);
	}

	//serial_processCommand(length);
	return retval;
}

static void serial_stateChange(usb_cdc_control_signal_t state)
{
	if (state.rs232.DTR != 0)
	{
		connected = 1;
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)serial_read);

		//menu_init();
	}
	else
	{
		connected = 0;
	}
}

uint8_t serial_init(void)
{
	if (cdcdf_acm_is_enabled() == 0)
	{
		return 1;
	}

	cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)serial_stateChange);

	return 0;
}

uint8_t serial_write(uint8_t data)
{
	if (connected == 0)
	{
		return 1;
	}

	return cdcdf_acm_write(&data, 1);
}

uint8_t serial_writeBuffer(uint8_t *data, uint32_t length)
{
	if (connected == 0)
	{
		return 1;
	}

	return cdcdf_acm_write(data, length);
}

uint8_t serial_writeString(const char *string)
{
	uint32_t length = strlen(string);
	return serial_writeBuffer((uint8_t *)string, length);
}
