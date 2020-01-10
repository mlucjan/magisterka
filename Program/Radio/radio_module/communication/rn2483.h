/*
 * rn2483.h
 *
 *  Created on: 24 lis 2019
 *      Author: michal
 */

#ifndef COMMUNICATION_RN2483_H_
#define COMMUNICATION_RN2483_H_

#include "driverlib.h"

#define LORA_RX_BUFFER_SIZE 60
#define MAX_PAYLOAD_SIZE 255

void clear_lora_rx_buffer();
//uint8_t buffer_compare(const uint8_t* buf1, const uint8_t* buf2);
void buffer_to_hex_string(uint8_t* src_buffer, uint16_t src_buffer_size,
                          uint8_t* dest_buffer);
void lora_command(uint8_t* data, const uint8_t* expected_response,
                  uint16_t response_timeout);
void init_lora();
void lora_transmit_data_p2p(uint8_t* payload, uint8_t payload_size,
                            uint16_t first_response_timeout,
                            uint16_t second_response_timeout);
void lora_wake_up();
void lora_sleep();

#endif /* COMMUNICATION_RN2483_H_ */
