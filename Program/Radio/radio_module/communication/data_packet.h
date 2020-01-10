/*
 * data_packet.h
 *
 *  Created on: 28 gru 2019
 *      Author: michal
 */

#ifndef COMMUNICATION_DATA_PACKET_H_
#define COMMUNICATION_DATA_PACKET_H_

//#include <stdint.h>
#include "driverlib.h"

#define PACKET_SIZE_BYTES 28

typedef union{
    uint8_t bytearray[PACKET_SIZE_BYTES];
    struct{
//        uint8_t packetSizeBytes;
        uint32_t micMax;
        uint32_t micMin;
        float lux;
        float iaq;
        float temperature;
        float humidity;
        float pressure;

    };
}dataPacket;

extern const dataPacket packetInitValue;

#endif /* COMMUNICATION_DATA_PACKET_H_ */
