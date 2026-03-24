/*
 * Core.c
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */


#include "Core.h"
#include "user_config.h"
#include "serialTasks.h"


uint8_t SetOperationMode(OPERATION_MODE_TYPE_ENUM mode)
{
    uint8_t changed = false;
    switch(mode)
    {
    case OPM_HIBERNATE:
    case OPM_CONFIG:
    case OPM_HIBERNATE_PRERUN:
    case OPM_NORMAL:
    case OPM_TEST:
    case OPM_TEST_CONTINUOUS:
    case OPM_FLASH_FIRMWARE:
    case OPM_BOOT_PASSTHRU:
    case OPM_RESET:
        //JGW_CONF .RunMode = mode;
        changed = true;
        break;
    default:
        break;

    }
    return changed;
}
