/*
 * data_packet.h
 *
 *  Created on: 28 gru 2019
 *      Author: michal
 */

#ifndef COMMUNICATION_DATA_PACKET_H_
#define COMMUNICATION_DATA_PACKET_H_
typedef union{
    uint8_t bytearray[28];
    struct{
        uint32_t mic_max;
        uint32_t mic_min;
        float lux;
        float iaq;
        float temperature;
        float humidity;
        float pressure;

    };
}dataPacket;

#endif /* COMMUNICATION_DATA_PACKET_H_ */
