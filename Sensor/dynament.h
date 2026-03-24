/*
 * dynament.h
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#ifndef SENSOR_DYNAMENT_H_
#define SENSOR_DYNAMENT_H_

#include "SerialCommon.h"
#include "dynament_comms.h"

//******************************************************************************
// definitions
//******************************************************************************

/* Packet size definitions */
#define DYNAMENT_MAX_PACKET_SIZE        300

#define DYNAMENT_MIN_VALID 0
#define DYNAMENT_MAX_VALID 2000

// function prototypes
//******************************************************************************

// Message/COMMS frame processing
SENSOR_PROCESS_MSG_TYPE dynament_response(void);
uint8_t dynament_got_msg(void);
uint8_t dynament_handle_data(uint8_t chr);

void SendDynamentRequest(void);
bool SendDynamentPacket(uint8_t cmd, uint8_t variableID, uint8_t dlen, uint8_t *dataPtr);

void PrintGasSensorReadings(void);
SENSOR_PROCESS_MSG_TYPE ReadLiveData2Response(uint8_t *dataPtr, int len);

uint16_t dynamentGetLatestGasValue(void);


#endif /* SENSOR_DYNAMENT_H_ */
