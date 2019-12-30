/*
 * rn2483.c
 *
 *  Created on: 24 lis 2019
 *      Author: michal
 */
#include "rn2483.h"
#include "mcu_config/peripherals.h"
#include "Board.h"
#include <stdlib.h>

//RN2483 response strings
const uint8_t* resp_ok = "ok\r\n";
const uint8_t* resp_invalid_param = "invalid_param\r\n";
const uint8_t* resp_max_mac_pause = "4294967245\r\n";

//RN2483 commands and responses
const uint8_t* command_header = "radio tx ";
const uint8_t* first_response = "ok\r\n";
const uint8_t* second_response = "radio_tx_ok\r\n";

uint8_t rx_buf[LORA_RX_BUFFER_SIZE];
uint8_t rx_index = 0, rx_complete = 0x00;
uint8_t rxdata = 0;

uint8_t hex_payload[MAX_PAYLOAD_SIZE + 2];
uint8_t nibble1 = 0;
uint8_t nibble2 = 0;

uint8_t tmp = 0;
uint8_t timeouted = 0;

void clear_lora_rx_buffer(){
    int i;
    for(i = 0; i < LORA_RX_BUFFER_SIZE; i++)
    {
        rx_buf[i] = 0;
    }
}

uint8_t buffer_compare(const uint8_t* buf1, const uint8_t* buf2){
    int i = 0;
    while((buf1[i] != '\n') || (buf1[i] != '\n')){
        if(buf1[i] != buf2[i]) {
            return 1;
        }
        i++;
    }
    return 0;
}

void buffer_to_hex_string(uint8_t* src_buffer, uint16_t src_buffer_size,
                              uint8_t* dest_buffer){
    uint16_t i = 0;
    uint16_t last_conv_index = src_buffer_size - 1;

//    if(buffer[last_conv_index] == '\n' && buffer[last_conv_index - 1] == '\r'){
//        dest_buffer[last_conv_index] = '\n';
//        dest_buffer[last_conv_index - 1] = '\r';
//        last_conv_index -= 2;
//    }
    while(i <= last_conv_index){
        nibble1 = (src_buffer[i] & 0xf0) >> 4; //(*(buffer+i) & 0xf0) >> 4;
        if(nibble1 <= 9){
            dest_buffer[2*i] = nibble1 + 0x30; //0x30 - 0x39 is 0-9 ASCII range
        }
        else{
            dest_buffer[2*i] = nibble1 + 0x37; //A - F range is 7 values further in the ASCII table
        }
        nibble2 = (src_buffer[i] & 0x0f); //(*(buffer+i) & 0x0f);
        if(nibble2 <= 9){
            dest_buffer[2*i+1] = nibble2 + 0x30; //0x30 - 0x39 is 0-9 ASCII range
        }
        else{
            dest_buffer[2*i+1] = nibble2 + 0x37; //A - F range is 7 values further in the ASCII table
        }
        i++;
    }
    dest_buffer[2*last_conv_index + 2] = '\r'; //last converted digit is on 2*last_conv_index+1
    dest_buffer[2*last_conv_index + 3] = '\n';
}

void lora_command(uint8_t* data, const uint8_t* expected_response, uint16_t response_timeout){
    clear_lora_rx_buffer();
    rx_complete = 0x00;
    rx_index = 0;
    //Transmit data from the buffer
    uint8_t buffer_sent = 0x00, i = 0;
    while(!buffer_sent){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A0_BASE, EUSCI_A_UART_BUSY)){
            ;
        }
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, data[i]);
        if(data[i] == '\n') buffer_sent = 0x01;
        else i++;
    }

    //Wait for RN2483 response message
    uint16_t time = 0;
    while(rx_complete == 0x00){
        time++;
        if(time == response_timeout) break;
    }

    if(rx_complete == 0x01){
        if(buffer_compare(rx_buf, expected_response) != 0){
            GPIO_setOutputHighOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        }
    }

}

void lora_init(){
    //RN2483 reset
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
    __delay_cycles(320); //hold for 20us
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);

    //Wait for RN2483 to wake up
    __delay_cycles(4000000);

    //init UART module
    init_lora_uart();

    //9600 autobaud detection
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, 0x00);//break condition for 9600 baud rate
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, 0x55);

    __delay_cycles(16000000);

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
    case LORA_SELECTED:
        __enable_interrupt();
        //lora_command -> set freq, mod etc
        lora_command("radio set mod lora\r\n", resp_ok, 10000);
        __delay_cycles(4000000);
        lora_command("radio set freq 868000000\r\n", resp_ok, 10000);
        __delay_cycles(4000000);
        lora_command("radio set pwr 14\r\n", resp_ok, 10000);
        __delay_cycles(4000000);
        lora_command("mac pause\r\n", resp_max_mac_pause, 10000);
        __delay_cycles(4000000);
        break;
    case BLE_SELECTED:
        lora_command("sys sleep 4294967296\r\n", resp_ok, 10000);
    default:
        break;
    }

}

void lora_transmit_data_p2p(uint8_t* payload, uint8_t payload_size, uint16_t first_response_timeout,
                            uint16_t second_response_timeout){

    GPIO_setOutputLowOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);

    clear_lora_rx_buffer();
    rx_complete = 0x00;
    rx_index = 0;

    //Transmit data from the buffer
    uint8_t header_sent = 0x00;
    uint8_t payload_sent = 0x00;
    uint8_t i = 0;

    buffer_to_hex_string(payload, payload_size, hex_payload);

    while(!header_sent){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A0_BASE, EUSCI_A_UART_BUSY)){
            ;
        }
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, command_header[i]);
        if(i == (sizeof("radio tx ") - 2)){ //sizeof("radio tx ") = 10, max index = 8
            header_sent = 0x01;
            i = 0;
        }
        else i++;
    }

    while(!payload_sent){
        while(EUSCI_A_UART_queryStatusFlags(EUSCI_A0_BASE, EUSCI_A_UART_BUSY)){
            ;
        }
        EUSCI_A_UART_transmitData(EUSCI_A0_BASE, hex_payload[i]);
        if(hex_payload[i] == '\n' || i == MAX_PAYLOAD_SIZE){
            payload_sent = 0x01;
            i = 0;
        }
        else i++;
    }

    //Wait for RN2483 1st response message
    uint16_t time = 0;
    while(rx_complete == 0x00){
        time++;
        if(time == first_response_timeout) {
            break;
        }
        __delay_cycles(160000);
    }

    if(rx_complete == 0x01){
        if(buffer_compare(rx_buf, first_response) != 0){
            GPIO_setOutputHighOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        }
    }

    rx_complete = 0;

    //Wait for RN2483 2nd response message
    time = 0;
    while(rx_complete == 0x00){
        time++;
        if(time == second_response_timeout){
            timeouted = 1;
            break;
        }
        __delay_cycles(160000);
    }

    if(rx_complete == 0x01){
//        tmp = 0;
        if(buffer_compare(rx_buf, second_response) != 0){
            GPIO_setOutputHighOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED);
        }
    }

}


//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void EUSCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
//            rxdata = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
//            EUSCI_A_UART_transmitData(EUSCI_A1_BASE, rxdata);
            rx_buf[rx_index] = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);
            if(rx_index > 1){
                if(rx_buf[rx_index] == '\n' && rx_buf[rx_index-1] == '\r'){
                    rx_complete = 0x01;
                    rx_index = 0;
                    break;
                }
            }
            rx_index++;
            break;
       case USCI_UART_UCTXIFG: break;
       case USCI_UART_UCSTTIFG: break;
       case USCI_UART_UCTXCPTIFG: break;
    }
}
