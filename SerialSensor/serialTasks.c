/*
 * serialTasks.c
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#include <stdbool.h>
#include "hal_gpio.h"
#include "user_config.h"
#include "studiolib.h"
#include "serialTasks.h"
#include "SerialCommon.h"
#include "dynament.h"
#include "Core.h"
#include <stdint.h>

#define SERIAL_INIT_TIMEOUT     240         // Time to complete initialisation
#define SERIAL_TIMEOUT          30          // Serial tasks state completion timeout counter


// Initialise the Dynament Sensor Module

uint8_t InitialiseDynamentSensorTask(void)
{
    static uint8_t timeout = SERIAL_TIMEOUT;

    static SENSOR_PROCESS_MSG_TYPE res;
    static INIT_SENSOR_TYPE_ENUM state = INIT_START_SENSOR;
    static uint8_t retries = 2;

    switch(state)
    {
    case INIT_START_SENSOR:
        enable_module();
        DEBUG_STRING("\r\nInitialising - checking Dynamnet Sensor\r\n");
        state = INIT_SEND_REQUEST;
        retries = 2;
        break;
    case INIT_SEND_REQUEST:
        timeout = SERIAL_TIMEOUT;
        SendDynamentRequest();
        state = INIT_RECEIVE_DATA;
        break;
    case INIT_RECEIVE_DATA:
        res = dynament_response();
        if(dynament_got_msg() && (res != NO_RESPONSE))
        {
            if(res >= NEW_DATA_OUTLIER)
            {
                //PrintGasSensorReadings();
                state = INIT_FINISH;
            }
            else if (res == INVALID_DATA)
            {
                state = INIT_SEND_REQUEST;
            }
        }
        else
        {
            if(--timeout == 0)
            {
                if(--retries == 0)
                {
                    state = INIT_ERROR;
                }
                else
                {
                    state = INIT_SEND_REQUEST;
                }
            }
        }
            break;
    case INIT_FINISH:
        //JGW.ActList.InitialiseModule = false;
    default:
        disable_module();
        state = INIT_START_SENSOR;
        break;
        }
        return timeout;

}
