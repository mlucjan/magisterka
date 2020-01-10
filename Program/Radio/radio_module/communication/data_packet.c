/*
 * data_packet.c
 *
 *  Created on: 29 gru 2019
 *      Author: michal
 */
#include "data_packet.h"

const dataPacket packetInitValue = { //.packetSizeBytes = PACKET_SIZE_BYTES,
                      .micMax = 0,
                      .micMin = 0,
                      .lux = -1,
                      .iaq = -1,
                      .temperature = 999,
                      .humidity = -1,
                      .pressure = -1};
