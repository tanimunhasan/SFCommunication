/*
 * hal_gpio.c
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */



#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <gpio.h>
#include "hal_gpio.h"
#include "driverlib.h"
#include "hal_uart.h"
#include "studiolib.h"


void enable_sensor_power(void);
void disable_sensor_power(void);

void hal_gpio_Init(void)
{
    /* GPIO Configuration
     * All pins initialised as output low, except for:
     *  P1[0:1] Battery Voltage (Analogue A0 and A1)
     *  P1[2:5] External I/O pins
     *  P1[6]   PwerKey (NB-IoT) or Boot (Lora)
     *  P1[7]   -------------------------------
     *  P2[0:2] Debug Uart UCA3 and BSL
     *  P2[3:7] -------------------------------
     *  P3[1]   Test Pad
     *  P3[3]   Reed Switch input
     *  P3[4]   Batt/Ext status
     *  P4[0]   WMBus Power EN
     *  P4[1]   WMBus PKEY (not used)
     *  P4[2]   WMBus RST
     *  P4[3:4] WMBus Uart UCA0
     *  P5[0:1] Modem UART UCA2
     *  P5[3]   Modem Interrupt - Not used, set as input
     *  P5[4]   Modem Reset
     *  P5[4:7] SPI interface
     *  P6[0]   Green LED
     *  P6[1:7] -------------------------------
     *  P7[0:7] -------------------------------
     *  PJ[4:5] LFXT
     */

    // Set the output low on ports
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    // Set the output direction on ports
    GPIO_setAsOutputPin(GPIO_PORT_P1,                     GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|          GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|          GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|          GPIO_PIN2|                    GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|          GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|                    GPIO_PIN6|GPIO_PIN7);

    // --------------  Peripheral Definitions ----------------------------- //

    // P3: IRQ
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN3|GPIO_PIN4);
    GPIO_selectInterruptEdge(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN3|GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN3|GPIO_PIN4);
#if (ENABLE_BATTERY_INTERRUPT)
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN3|GPIO_PIN4);
#else
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN1|GPIO_PIN3);
#endif

    // Configure LFXT GPIO pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ, GPIO_PIN4|GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    // Analogue inputs P1.0 = A0 and P1.1 = A1
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1, GPIO_TERNARY_MODULE_FUNCTION);

    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    PMM_unlockLPM5();
}

void enable_sensor_power(void)
{
    DEBUG_STRING("\r\nEnabling SERIAL_MODULE\0");
    SERIAL_ENABLE();
}
void enable_module(void)
{
    enable_sensor_power();
}

void disable_sensor_power(void)
{
    SERIAL_DISABLE();
    DEBUG_STRING("Serial_Module Disabled\r\n\0");
}
void disable_module(void)
{
    disable_sensor_power();
}
