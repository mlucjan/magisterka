/*
 * sensing_module.c
 *
 *  Created on: 30 gru 2019
 *      Author: michal
 */
#include "sensing_module_comm.h"
#include "Board.h"
int i,j = 0;
int timeout = 0;

void clear_buffer(uint8_t* buffer, uint8_t size){
    uint8_t index = 0;
    for(index = 0; index < size; index++){
        buffer[i] = 0;
    }
}
uint8_t sensor_data_ready(){
    uint8_t pin = GPIO_getInputPinValue(GPIO_PORT_SENS, GPIO_PIN_SENS_DATA_RDY);
    if(pin == GPIO_INPUT_PIN_HIGH){
//        i = 0;
        return 1;
    }
    else{
//        i = 0;
        return 0;
    }
}

void read_sensor_data(uint8_t* packet_bytearray, uint8_t packet_size_bytes){
    clear_buffer(packet_bytearray, packet_size_bytes);
    for(i = 0; i < packet_size_bytes; i++){
      EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xff);
      for(timeout = 0; timeout < 100; timeout++){
          if(!EUSCI_B_SPI_isBusy(EUSCI_B0_BASE)) break;
          __delay_cycles(120);
      }
//              __delay_cycles(12000);
      packet_bytearray[i] = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
    }
}
