#ifndef APP_SPI_H
#define APP_SPI_H

void SPI_write(uint8_t data);
uint8_t SPI_read(void);
uint8_t SPI_transfer(uint8_t data);
void SPI_writeBuffer(uint8_t *data, uint8_t length);
void SPI_readBuffer(uint8_t *data, uint8_t length);
void SPI_start(void);
void SPI_end(void);

#endif
