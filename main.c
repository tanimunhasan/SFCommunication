/*
 * main.c
 *
 *  Created on: 20 Feb 2026
 *      Author: B4T
 */

#include <stdio.h>
#include <msp430.h>
#include <string.h>
#include <stdbool.h>
#include "hal_uart.h"
#include "hal_gpio.h"
#include "main.h"
#include "studiolib.h"
#include "hal_system.h"


void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;         // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;
    clockConfigure();
    uart_init();
    initGLED();
    reed_init_irq();
    DEBUG_STRING("Hello SFCommunication\n");

    __enable_interrupt();

    while(1)
        {
        if (g_reed_tap)
        {
            g_reed_tap = false;

            DEBUG_STRING("\r\nMagnet Tapped");

            P6OUT &= ~BIT0;          // LED OFF
            delay_ms(1000);

        }

        }
}
