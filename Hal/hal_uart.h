/*
 * hal_uart.h
 *
 *  Created on: 20 Feb 2026
 *      Author: B4T
 */

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef HAL_HAL_UART_H_
#define HAL_HAL_UART_H_
/* Define UART (eUSCI) base address */
#define MODULE_WRITE_STR(x,L)  module_Write_Str(x,L)

extern volatile bool g_reed_tap;


void delay_ms(unsigned int ms);
void uart_init(void);
void delay_ms(unsigned int ms);
void clockConfigure(void);
void initGLED(void);
void toggleGLED(void);
void GLED_on(void);
void GLED_off(void);
bool reed_is_pressed(void);
void reed_init_irq(void);


#endif /* HAL_HAL_UART_H_ */
