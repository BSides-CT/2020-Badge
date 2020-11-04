/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef ATMEL_START_PINS_H_INCLUDED
#define ATMEL_START_PINS_H_INCLUDED

#include <hal_gpio.h>

// SAMD11 has 8 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7

#define LED1 GPIO(GPIO_PORTA, 2)
#define LED2 GPIO(GPIO_PORTA, 3)
#define LED3 GPIO(GPIO_PORTA, 4)
#define LED4 GPIO(GPIO_PORTA, 5)
#define LED5 GPIO(GPIO_PORTA, 6)
#define LED6 GPIO(GPIO_PORTA, 7)
#define LED7 GPIO(GPIO_PORTA, 8)
#define LED8 GPIO(GPIO_PORTA, 9)
#define IRQ_OUT GPIO(GPIO_PORTA, 14)
#define IRQ_IN GPIO(GPIO_PORTA, 15)
#define SPI_SS GPIO(GPIO_PORTA, 23)

#endif // ATMEL_START_PINS_H_INCLUDED
