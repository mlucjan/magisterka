/*
 * sensing_module.c
 *
 *  Created on: 30 gru 2019
 *      Author: michal
 */
#include "sensing_module.h"
//#include "driverlib.h"
int i,j = 0;

uint8_t sensor_data_ready(){
    uint8_t pin = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN0);
    if(pin == GPIO_INPUT_PIN_HIGH){
        i = 0;
        return 1;
    }
    else{
        i = 0;
        return 0;
    }
}

void read_sensor_data(uint8_t* packet_bytearray, uint8_t packet_size_bytes){
    for(i = 0; i < packet_size_bytes; i++){
        EUSCI_A_SPI_transmitData(EUSCI_B0_BASE, 0xff);

        for(j=0; j < 10; j++){
            if(!EUSCI_A_SPI_isBusy(EUSCI_B0_BASE)) break;
            __delay_cycles(160);
        }

        packet_bytearray[i] = EUSCI_A_SPI_receiveData(EUSCI_B0_BASE);
    }
}


