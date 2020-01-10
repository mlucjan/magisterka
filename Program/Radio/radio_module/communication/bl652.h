/*
 * bl652.h
 *
 *  Created on: 24 lis 2019
 *      Author: michal
 */

#ifndef COMMUNICATION_BL652_H_
#define COMMUNICATION_BL652_H_

#define BLE_BUFFER_SIZE 70

#include "driverlib.h"
uint8_t find_pattern(uint8_t* searched_buffer, const uint8_t* pattern,
                       uint8_t pattern_length, uint8_t start_index);
void init_BLE();
void BLE_transmit_data(uint8_t* payload, uint8_t payload_size);

#endif /* COMMUNICATION_BL652_H_ */
