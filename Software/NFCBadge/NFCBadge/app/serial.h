#ifndef APP_SERIAL_H
#define APP_SERIAL_H

#include <stdint.h>

uint8_t serial_init(void);
uint8_t serial_write(uint8_t data);
uint8_t serial_writeBuffer(uint8_t *data, uint32_t length);
uint8_t serial_writeString(const char *string);

#endif
