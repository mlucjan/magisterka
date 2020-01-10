/*
 * sensing_module.h
 *
 *  Created on: 30 gru 2019
 *      Author: michal
 */

#ifndef COMMUNICATION_SENSING_MODULE_COMM_H_
#define COMMUNICATION_SENSING_MODULE_COMM_H_
//#include <stdint.h>
#include "driverlib.h"
uint8_t sensor_data_ready();
void read_sensor_data(uint8_t* dest_bytearray, uint8_t packet_size_bytes);

#endif /* COMMUNICATION_SENSING_MODULE_COMM_H_ */
