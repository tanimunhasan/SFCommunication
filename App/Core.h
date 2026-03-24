/*
 * Core.h
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#ifndef APP_CORE_H_
#define APP_CORE_H_

#include <msp430.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "user_config.h"

// ***********************************************************
// type definitions
// ***********************************************************
#define DEV_ID_LEN              8
#define KEY_ID_LEN              16


typedef enum _MDM_STATE_ENUM_
{
    STATE_RESET_FACTORY = -2,
    STATE_RESET,
    STATE_IDLE,
    STATE_INIT,
    STATE_CONNECT,
    STATE_ACTIVATE,
    STATE_CONFIGURE,
    STATE_READY,
    STATE_WAKEUP,
    STATE_SEND,
    STATE_SEND_CALLBACK_WAIT,
    STATE_SLEEP,
    STATE_SHUTDOWN,
    STATE_FINISH,
    STATE_FINISH_ERROR
}MDM_STATE_ENUM;

typedef enum
{
    OPM_HIBERNATE,
    OPM_CONFIG,
    OPM_HIBERNATE_PRERUN,
    OPM_NORMAL,
    OPM_TEST,                   // nominally runs for 60 seconds
    OPM_TEST_CONTINUOUS,        // run continually
    OPM_BOOT_PASSTHRU = 0xFE,   // reboot into pass through mode
    OPM_RESET = 0xFF,            // FULL system/data reset to factory defaults
    OPM_FLASH_FIRMWARE = 250,   // 0xFA Reboot and initiate boot loader
}OPERATION_MODE_TYPE_ENUM;

typedef union
{
    uint8_t value;
    struct
    {
        unsigned msg_type:   4;
        unsigned CallBack:   1;
        unsigned JoinMode:   1;  // LoRajoin 0=OTAA (default), 1=ABP
        unsigned AckConfirm: 1;  // 0=Send ACK required, 1=NoConfirm
        unsigned Unused:     1;
    };
}MSG_TYPE_STRUCT;

typedef struct
{
    //uint8_t DeviceID[DEV_ID_LEN + 1];   // 24 bytes, DevID
    uint8_t SIM_PAC[KEY_ID_LEN + 1];    // 48 bytes, PAC or SIM
    uint8_t RSSI;                       //  1 bytes
                                  // Total 73 bytes
                                            // Total 73 bytes
    uint8_t Region;                     // SigFox Total 1 byte
    MDM_STATE_ENUM mdm_state;               // (Total + 2) bytes
    uint16_t MessageCount;                  // (Total + 2) bytes
    MSG_TYPE_STRUCT msgTypeFlags;           // (Total + 1) bytes
}RADIO_TYPE;


typedef union
{
    uint16_t Value;
    struct
    {
        uint8_t curretState;
        uint8_t signalEvents;
    };
    struct
    {
        unsigned Radio_Initialised:     1;
        unsigned Module_Initialised:    1;
        unsigned LED_Hibernate_Blip_On: 1;
        unsigned ListeningEnabled:      1;
        unsigned stateOther:            4;

        // Signal flags
        unsigned BatteryInterrupt:      1;
        unsigned TestPadInterrupt:      1;
        unsigned MagnetTapped:          1;
        unsigned ConfigChanged:         1;  // Command (protocol) changes processed
        unsigned MODULEchangeEvent:     1;  // MBUS or SERIAL module signalled event
        unsigned WDTEvent:              1;
        unsigned signalOther:           2;

    };
}STATUS_TYPE;

typedef struct
{
    STATUS_TYPE Status;
    uint16_t Battery;
    uint16_t ExtSupply;
}JGW_TYPE;

// Basic (base level) message (BCD nibble) value types
#define STATUS_PACKET        0x00
#define ALERT_PACKET         0x01
#define INFORMATION_PACKET   0x02
#define RESERVERED_03           0x03
#define GEOPOSITIONAL_PACKET 0x04
#define RESERVERED_05           0x05
#define RESERVERED_06           0x06
#define RESERVERED_07           0x07
#define DATA_PACKET          0x08
#define RESERVERED_09           0x09
#define RESERVERED_10           0x10
#define RESERVERED_11           0x11
#define RESERVERED_12           0x12
#define RESERVERED_13           0x13
#define RESERVERED_14           0x14
#define RESERVERED_15           0x15


typedef union
{
    uint16_t Val;
    struct
    {
        uint8_t moduleTasks;
        uint8_t transmitTasks;
    };
    struct
    {
        // meter tasks - high priority
        unsigned InitaliseModule:1;     // MBUS or Serial
        unsigned RegTestMeter:1;
        unsigned RegisterMeter:1;
        unsigned NewMessage:1;
        unsigned WaitForMsg:1;
        unsigned SampleAnalogue:1;
        unsigned FinshedStartUp:1;
        unsigned sam:1;

        // active transmit task - mid priority task
        unsigned SendStatus:1;
        unsigned SendData:1;
        unsigned RetrySend:1;
        unsigned SendMeterList:1;
        unsigned QuickResetStart:1;
        unsigned tran:4;
    };
}ACTIONS_TYPE;


#endif /* APP_CORE_H_ */
