/*
 * bl652.c
 *
 *  Created on: 24 lis 2019
 *      Author: michal
 */
#include "bl652.h"
#include "mcu_config/peripherals.h"
#include "Board.h"

//BLE receive buffer
uint8_t rxBufBLE[BLE_BUFFER_SIZE] = {0};
//execution state flags
uint8_t rxIndexBLE = 0;
uint8_t rxCompletedBLE = 0;
uint8_t BLEConnected = 0;
//constants
const uint8_t* connectPattern = "CONNECT";

uint8_t find_pattern(uint8_t* searched_buffer, const uint8_t* pattern,
                       uint8_t pattern_length, uint8_t start_index){
    int i = 0;
    for(i = start_index; i < start_index + pattern_length; i++){
        if(searched_buffer[i] != pattern[i - start_index]) {
            return 1;
        }
    }
    return 0;
}

void init_BLE(){
    rxCompletedBLE = 0;

    uint8_t dipsw = read_dipswitch_setting();
    switch(dipsw){
    case NONE_SELECTED:
        GPIO_setOutputHighOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        while(dipsw == NONE_SELECTED){
            dipsw = read_dipswitch_setting();
        }
        GPIO_setOutputLowOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        break;
    case BOTH_SELECTED:
    case BLE_SELECTED:
        __enable_interrupt();
        while(!BLEConnected){
            GPIO_toggleOutputOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
            if(rxCompletedBLE){
                if((find_pattern(rxBufBLE, connectPattern, 7, 24) == 0) || (find_pattern(rxBufBLE, connectPattern, 7, 1) == 0)){
                    BLEConnected = 1;
                    rxCompletedBLE = 0;
                    GPIO_setOutputLowOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
                }
            }
            __delay_cycles(6000000);
        }
        break;
    case LORA_SELECTED:
        break;
    default:
        break;
    }
}

void BLE_transmit_data(uint8_t* payload, uint8_t payload_size){
    uint8_t i = 0;
    for(i = 0; i < payload_size; i++){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A1_BASE, EUSCI_A_UART_BUSY)){
            ;
        }
        EUSCI_A_UART_transmitData(EUSCI_A1_BASE, payload[i]);
    }
}

//******************************************************************************
//
//This is the USCI_A1 interrupt vector service routine. (BLE UART)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void EUSCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:

            rxBufBLE[rxIndexBLE] = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);
            if(rxIndexBLE > 23){//1){
                if(rxBufBLE[rxIndexBLE] == '\r'){
                    rxCompletedBLE = 1;
                    rxIndexBLE = 0;
                    break;
                }
            }
            rxIndexBLE++;
            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
    }
}
