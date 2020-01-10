/*
 * radio_comm.h
 *
 *  Created on: 10 sty 2020
 *      Author: michal
 */

#ifndef COMMUNICATION_RADIO_COMM_H_
#define COMMUNICATION_RADIO_COMM_H_
#include "driverlib.h"

void init_radio();
void send_data_radio(uint8_t* payload, uint8_t payload_size);


#endif /* COMMUNICATION_RADIO_COMM_H_ */
