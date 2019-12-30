/*
 * utils.c
 *
 *  Created on: 8 gru 2019
 *      Author: michal
 */
#include "utils.h"
uint8_t* buffer_to_hex_string(uint8_t* buffer, uint16_t buffer_size){
    uint8_t result[2*buffer_size];
    uint16_t i = 0, last_index = buffer_size - 1;
    uint8_t nibble1 = 0;
    uint8_t nibble2 = 0;

    if(buffer[last_index] == '\n' && buffer[last_index - 1] == '\r') last_index -= 2;
    while(i <= last_index){
        nibble1 = (buffer[i] & 0xf0) >> 4;
        if(nibble1 <= 9) result[2*i] = nibble1 + 0x30; //0x30 - 0x39 is 0-9 ASCII range
        else result[2*i] = nibble1 + 0x37; //A - F range is 7 values further in the ASCII table

        nibble2 = (buffer[i] & 0x0f);
        if(nibble1 <= 9) result[2*i+1] = nibble2 + 0x30; //0x30 - 0x39 is 0-9 ASCII range
        else result[2*i+1] = nibble2 + 0x37; //A - F range is 7 values further in the ASCII table
    }
    return result;
}
