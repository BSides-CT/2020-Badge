#include <atmel_start.h>
#include "application.h"

void SPI_write(uint8_t data)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	io_write(io, &data, 1);
}

uint8_t SPI_read(void)
{
	uint8_t retval = 0;

	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	io_read(io, &retval, 1);

	return retval;
}

uint8_t SPI_transfer(uint8_t data)
{
	uint8_t retval = 0;

	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	io_write(io, &data, 1);
	io_read(io, &retval, 1);

	return retval;
}

void SPI_writeBuffer(uint8_t *data, uint8_t length)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	io_write(io, data, length);
}

void SPI_readBuffer(uint8_t *data, uint8_t length)
{
	struct io_descriptor *io;
	spi_m_sync_get_io_descriptor(&SPI_0, &io);

	io_read(io, data, length);
}

void SPI_start(void)
{
	gpio_set_pin_level(SPI_SS, false);
}

void SPI_end(void)
{
	gpio_set_pin_level(SPI_SS, true);
}
