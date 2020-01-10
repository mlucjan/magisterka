/*
 * radio_comm.c
 *
 *  Created on: 10 sty 2020
 *      Author: michal
 */

#include "radio_comm.h"
#include "rn2483.h"
#include "bl652.h"
#include "mcu_config/peripherals.h"
#include "Board.h"

uint8_t previousRadioChoice,currentRadioChoice = 0; //radio interface selection
extern uint8_t BLEConnected; //flag indicating if BL652 is connected to a host
void init_radio(){
    init_lora();
    init_BLE();
    currentRadioChoice = read_dipswitch_setting();
}

void send_data_radio(uint8_t* payload, uint8_t payload_size){
    previousRadioChoice = currentRadioChoice;
    currentRadioChoice = read_dipswitch_setting();
    switch(currentRadioChoice){

    case NONE_SELECTED:
        GPIO_setOutputHighOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        while(currentRadioChoice == NONE_SELECTED){
            currentRadioChoice = read_dipswitch_setting();
        }
        GPIO_setOutputLowOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        break;

    case LORA_SELECTED:
        if(previousRadioChoice == BLE_SELECTED){
            lora_wake_up();
        }
        lora_transmit_data_p2p(payload, payload_size, 1000, 1000);
        break;

    case BLE_SELECTED:
        if(previousRadioChoice != BLE_SELECTED){
            lora_sleep();
            init_BLE();
        }
        if(BLEConnected){
            BLE_transmit_data(payload, payload_size);
        }
        else{
            init_BLE();
            BLE_transmit_data(payload, payload_size);
        }
        break;

    case BOTH_SELECTED:
        if(previousRadioChoice == BLE_SELECTED){
            lora_wake_up();
        }
        lora_transmit_data_p2p(payload, payload_size, 1000, 1000);
        if(BLEConnected){
            BLE_transmit_data(payload, payload_size);
        }
        else{
            init_BLE();
            BLE_transmit_data(payload, payload_size);
        }
        break;

    default:
        break;
    }
}
