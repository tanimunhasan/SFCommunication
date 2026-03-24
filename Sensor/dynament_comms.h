/*
 * dynament_comms.h
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#ifndef SENSOR_DYNAMENT_COMMS_H_
#define SENSOR_DYNAMENT_COMMS_H_

/* Packet constants */
#define READ_VAR                0x13
#define DLE                     0x10
#define WRITE_REQUEST           0x15
#define ACK                     0x16
#define NAK                     0x19
#define DAT                     0x1a

#undef  EOF
#define EOF                     0x1f

#define WRITE_PASSWORD_1 0xe5
#define WRITE_PASSWORD_2 0xa2

/* Variable IDs */
#define LIVE_DATA_SIMPLE        0x06
#define LIVE_DATA_2             0x2c

/* CRC calculation constants */
#define CRC_POLYNOMIAL          0x8005
#define CRC_VALUE_MASK          0x8000

// Comms type
#define DYNAMENT_PROTOCOL       1
#define COMMS_PROTOCOL          DYNAMENT_PROTOCOL

// Checksum type
#define CSUM_STANDARD           0
#define CSUM_CRC                1
#define CSUM_TYPE               CSUM_STANDARD


#endif /* SENSOR_DYNAMENT_COMMS_H_ */
