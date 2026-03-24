/*
 * dynament.c
 *
 *  Created on: 23 Feb 2026
 *      Author: B4T
 */

#include<msp430.h>
#include <stdio.h>
#include<stdint.h>
#include <stdbool.h>

#include "user_config.h"

#include "hal_uart.h"
#include "dynament.h"
#include <math.h>

//******************************************************************************
// global STATE variables
//******************************************************************************

SENSOR_PROCESS_MSG_TYPE msg_response = NO_RESPONSE; // handle_data() processing state
uint8_t msg_received = 0;                           // handle_data() - received state

bool receivingPacket = false;
uint8_t D_rxBuffer[DYNAMENT_MAX_PACKET_SIZE];
unsigned int D_rxCount = 0;
bool DLEReceived = false;
bool waitingForCommand = false;
uint8_t command = 0;
bool EOFReceived = false;
uint16_t rcvCsum = 0;
uint16_t calcCsum = 0;
bool packetComplete = false;
bool csumError = false;
int csumByteReceived = 0;
bool packetNAKed = false;
uint8_t errorCode = 0;
bool packetACKed = false;
volatile bool latestPacketValid = false;

/*------------------------------------------- */
// Global variables for convert float to Integer
float gasValue1 = 0;
float gasValue2 = 0;

uint16_t intGasValue1 = 0;
uint16_t intGasValue2 = 0;
uint16_t latestStatus1 = 0;
uint16_t latestStatus2 = 0;

//******************************************************************************
// local prototypes
//******************************************************************************

uint16_t UpdateChecksum(uint16_t currentCRC, uint8_t newByte);
uint16_t UpdateCRCTab(uint16_t index);
void Reset(void);
void ProcessReceivedPacket(void);
void PacketSent(void);

//******************************************************************************
//
//******************************************************************************
uint16_t dynamentGetLatestGasValue(void)
{
    return intGasValue1;
}

//******************************************************************************
// Send Dynament Request packet
//******************************************************************************
void SendDynamentRequest(void)
{
    SendDynamentPacket(READ_VAR, LIVE_DATA_2, 0, 0);
    msg_received = false;
    msg_response = NO_RESPONSE;
}

//------------------------------------------------------------------------------
// get the processed_message response code to last message received
//------------------------------------------------------------------------------
SENSOR_PROCESS_MSG_TYPE dynament_response(void)
{
    return msg_response;
}

//------------------------------------------------------------------------------
// get the message received flag (when handle_data complete frame processed)
//------------------------------------------------------------------------------
uint8_t dynament_got_msg(void)
{
    return msg_received;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
uint16_t UpdateChecksum(uint16_t currentCRC, uint8_t newByte)
{
    uint16_t retVal;
    if(CSUM_TYPE == CSUM_CRC)
    {
        retVal = (uint16_t)((currentCRC << 8) ^ UpdateCRCTab((uint16_t)((currentCRC >> 8) ^ newByte)));
    }
    else
    {
        retVal = (uint16_t) (currentCRC + (uint16_t) newByte);
    }
    return retVal;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
uint16_t UpdateCRCTab(uint16_t index)
{
    uint16_t uiIndex;
    uint16_t uiCrcValue = 0, uiTempCrcValue;
    uiCrcValue = 0;
    uiTempCrcValue = (uint16_t)(((unsigned int)index) << 8);
    for(uiIndex = 0; uiIndex < 8; uiIndex++)
    {
        if (((uiCrcValue ^ uiTempCrcValue) & CRC_VALUE_MASK) > 0)
        {
            uiCrcValue = (uint16_t)(uint16_t)((uiCrcValue << 1) ^ (uint16_t)CRC_POLYNOMIAL);
        }
        else
        {
            uiCrcValue = (uint16_t)(unsigned int)(uiCrcValue << 1);
        }
        uiTempCrcValue = (uint16_t)(unsigned int)(uiTempCrcValue << 1);
    }
    return uiCrcValue;
}

//------------------------------------------------------------------------------
// Reset all the variables associated with a packet being received
//  - typically called when a packet has timed out or been aborted
//------------------------------------------------------------------------------
void Reset(void)
{
    receivingPacket = false;
    D_rxCount = 0;
    DLEReceived = false;
    waitingForCommand = false;
    command = 0;
    EOFReceived = false;
    rcvCsum = 0;
    calcCsum = 0;
    packetComplete = false;
    csumError = false;
    csumByteReceived = 0;
    packetNAKed = false;
    packetACKed = false;
    errorCode = 0;
    latestPacketValid = false;
}

//------------------------------------------------------------------------------
// Typically called after transmission send request command -
//------------------------------------------------------------------------------
//
void PacketSent(void)
{
    Reset();
}

//------------------------------------------------------------------------------
/* Function: Create a packet of data in the Dynament protocol
  Inputs : cmd - the type of packet (read or write variable typically)
           variableID - the id of the variable to be read or written
           dlen - the number of bytes of data within the packet
           dataPtr - pointer to array of bytes of the packet data
  Outputs: Returns true if packet sent else returns false
*/
//------------------------------------------------------------------------------
bool SendDynamentPacket(uint8_t cmd, uint8_t variableID, uint8_t dlen, uint8_t *dataPtr)
{
    unsigned int x;

    uint8_t txBuf[DYNAMENT_MAX_PACKET_SIZE];
    unsigned short txBufPtr = 0;
    uint16_t csum = 0;

    txBuf[txBufPtr++] = DLE;
    csum = UpdateChecksum(csum, DLE);
    txBuf[txBufPtr++] = cmd;
    csum = UpdateChecksum(csum, (uint16_t)cmd);
    if(cmd == READ_VAR)
    {
        txBuf[txBufPtr++] = variableID;
        csum = UpdateChecksum(csum, (uint16_t)variableID);
    }
    else if(cmd == WRITE_REQUEST)
    {
        txBuf[txBufPtr++] = WRITE_PASSWORD_1;
        csum = UpdateChecksum(csum, (uint16_t)WRITE_PASSWORD_1);

        txBuf[txBufPtr++] = WRITE_PASSWORD_2;
        csum = UpdateChecksum(csum, (uint16_t)WRITE_PASSWORD_2);

        txBuf[txBufPtr++] = variableID;
        csum = UpdateChecksum(csum, (uint16_t)variableID);
    }

    if(dlen > 0)
    {
        if(dlen == DLE)
        {
            txBuf[txBufPtr++] = DLE;
            csum = UpdateChecksum(csum, (uint16_t)DLE);
        }
        txBuf[txBufPtr++] = dlen;
        csum = UpdateChecksum(csum, (uint16_t)dlen);
        for(x = 0; x < dlen; x++)
        {
            if(dataPtr[x] == DLE)
            {
                txBuf[txBufPtr++] = DLE;
                csum = UpdateChecksum(csum,(uint16_t) DLE);
            }
            txBuf[txBufPtr++] = dataPtr[x];
            csum = UpdateChecksum(csum, (uint16_t)dataPtr[x]);
        }
    }
    txBuf[txBufPtr++] = DLE;
    csum = UpdateChecksum(csum, (uint16_t)DLE);

    txBuf[txBufPtr++] = EOF;
    csum = UpdateChecksum(csum, (uint16_t)EOF);

    // Append checksum (two bytes)
    txBuf[txBufPtr++] = (uint8_t)((csum >> 8) & 0x00ff);
    txBuf[txBufPtr++] = (uint8_t)(csum & 0x00ff);

    if(dlen > DYNAMENT_MAX_PACKET_SIZE)
        return false;

    MODULE_WRITE_STR(txBuf,txBufPtr);

    PacketSent();

    return true;
}

uint8_t dynament_handle_data(uint8_t chr)
{
    uint8_t res = 0;
    if (D_rxCount >= DYNAMENT_MAX_PACKET_SIZE)  // buffer full so reset - this is a bad packet
    {
        Reset();
    }

    if (chr == DLE && !EOFReceived)
    {
        if (!receivingPacket)
        {
            receivingPacket = true;
            D_rxCount = 0;
            D_rxBuffer[D_rxCount++] = chr;
            calcCsum = UpdateChecksum(calcCsum, chr);
            DLEReceived |= true;
        }
        else if (DLEReceived) // last char was a DLE so this is a uint8_t stuffed DLE character in the data (unless the checksum is being received which is not uint8_t stuffed
        {
            D_rxBuffer[D_rxCount++] = chr;
            calcCsum = UpdateChecksum(calcCsum, chr);
        }
        else  // add characters to array as per normal  but set DLE flag
        {
            DLEReceived = true;
            D_rxBuffer[D_rxCount++] = chr;
            calcCsum = UpdateChecksum(calcCsum, chr);
        }

    }
    else if (chr == EOF && DLEReceived && !EOFReceived)  // this is the end of frame
    {
        D_rxBuffer[D_rxCount++] = chr;
        calcCsum = UpdateChecksum(calcCsum, chr);
        EOFReceived = true;
        csumByteReceived = 0;
        rcvCsum = 0;
    }
    else if (EOFReceived)  // receiving the checksum
    {
        D_rxBuffer[D_rxCount++] = chr;
        ++csumByteReceived;
        if (csumByteReceived >= 2)  // end of checksum receiving and end of packet
        {
            rcvCsum = (uint16_t)((D_rxBuffer[D_rxCount - 2] * 0x100) + D_rxBuffer[D_rxCount - 1]);
            packetComplete = true;
            if (rcvCsum != calcCsum)
            {
                 csumError = true;
                 msg_response = FRAME_CRC_ERROR;
            }
            else
            {
                // Do nothing /OK CHECKSUM
            }
        }

    }
    else if (packetNAKed)  // the packet has returned an error - this is the last uint8_t which is an error code
    {
        errorCode = chr;
        packetComplete = true;
        D_rxBuffer[D_rxCount++] = chr;
    }
    else if (receivingPacket) // normal character - added to the buffer if a packet reception is in progress
    {
        if (DLEReceived && module_get_rx_len() >= 1) //
        {
            command = chr;
            if (command == NAK)
            {
                packetNAKed = true;
            }
            if (command == ACK)
            {
                packetACKed = true;
                packetComplete = true;
            }
        }
        D_rxBuffer[D_rxCount++] = chr;
        calcCsum = UpdateChecksum(calcCsum, chr);
    }

    if (chr != DLE) DLEReceived = false;

    if (packetComplete)  // a complete packet has actually been received  -act on it according to the various flags
    {
        {
            ProcessReceivedPacket();
            latestPacketValid = true;
            msg_received = true;
        }
        res = 1;
    }

    return res;
}

void ProcessReceivedPacket(void)
{
    uint8_t cmd = D_rxBuffer[1];
    uint8_t len = D_rxBuffer[2];
    unsigned int i;

#if (DEBUG_SENSOR_LEVEL)
    DEBUG_STRING("\r\nEntered ProcessReceivedPacket\r\n");

    DEBUG_STRING("Raw Packet: ");
    for(i = 0; i < D_rxCount; i++)
    {
        DEBUG_HEX(D_rxBuffer[i]);
    }
    DEBUG_STRING("\n");
#endif

    if(cmd == DAT)      // data packet response to a read variable received
    {
        uint8_t rcvData[200];
        for(i = 0; i < len && (i + 3) < D_rxCount; i++)
        {
            rcvData[i] = D_rxBuffer[i + 3];
        }

        msg_response = ReadLiveData2Response(rcvData, len);
    }
    else
    {
        // do nothing..
    }
}

SENSOR_PROCESS_MSG_TYPE ReadLiveData2Response(uint8_t *dataPtr, int len)
{
    // Get the status values from the message
    uint16_t statusVal1 = (uint16_t)((dataPtr[3] * 0x100) + dataPtr[2]);
    uint16_t statusVal2 = (uint16_t)((dataPtr[41] * 0x100) + dataPtr[40]);
    latestStatus1 = statusVal1;
    latestStatus2 = statusVal2;
    // Get the float value for gas reading 1
    uint32_t intVal = (uint32_t)((dataPtr[7] * 0x1000000) +
                                  (dataPtr[6] * 0x10000) +
                                  (dataPtr[5] * 0x100) +
                                  dataPtr[4]);
    float *fPtr = (float *)&intVal;
    gasValue1 = *fPtr;
    intGasValue1 = (uint16_t)gasValue1; //NOTE: Convert Float to Integer

    // Get the float value for gas reading 2
    intVal = (uint32_t)((dataPtr[15] * 0x1000000) +
                        (dataPtr[14] * 0x10000) +
                        (dataPtr[13] * 0x100) +
                        dataPtr[12]);
    fPtr = (float *)&intVal;
    gasValue2 = *fPtr;
    intGasValue2 = (uint16_t)gasValue2;  //NOTE: Convert Float to Integer
    if(latestStatus1 == 0 && latestStatus2 == 0)
    {
        if ( intGasValue1 <= DYNAMENT_MAX_VALID )
            return NEW_DATA;
        else
            return NEW_DATA_OUTLIER;
    }
    else
    {
        return INVALID_DATA;
    }
}

void PrintGasSensorReadings(void)
{
    //uint8_t buff[11] = {0};



}
