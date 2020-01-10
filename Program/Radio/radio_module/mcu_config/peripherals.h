/*
 * peripherals.h
 *
 *  Created on: 9 lip 2019
 *      Author: michal
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_
//#include <stdint.h>
#include "driverlib.h"

//Radio module selection constants
#define LORA_SELECTED 0x10
#define BLE_SELECTED 0x01
#define BOTH_SELECTED 0x11
#define NONE_SELECTED 0x00

void init_gpio();
void init_spi();
void init_lora_uart();
void init_ble_uart();
void init_uart();
uint8_t read_dipswitch_setting();


#endif /* PERIPHERALS_H_ */
