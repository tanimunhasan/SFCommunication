/*
 * hal_gpio.h
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#ifndef HAL_HAL_GPIO_H_
#define HAL_HAL_GPIO_H_

#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpio.h>
#include "Core.h"

/* Serial Enable & Serial Disable  */
#define REED_GetValue()         GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN3)
#define GRN_LED_TOGGLE()        GPIO_toggleOutputOnPin(GPIO_PORT_P6, GPIO_PIN0)
#define SERIAL_ENABLE()             GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN0)
#define SERIAL_DISABLE()            GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0)

extern void hal_gpio_Init(void);

extern void enable_module(void);
extern void disable_module(void);

#endif /* HAL_HAL_GPIO_H_ */
