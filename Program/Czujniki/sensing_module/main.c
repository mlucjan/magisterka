#include <stdlib.h>
#include "mcu_config/clock_system.h"
#include "mcu_config/peripherals.h"
#include "sensors/mic.h"
#include "sensors/opt3001.h"
#include "sensors/bme680/bsec_msp430_implementation.h"
#include "communication/data_packet.h"

//Measurement result variables
float luxResult = 0;
micMeasResult micResult;
extern struct bsec_output my_bsec_output;
return_values_init bsec_init_ret_val;

//System variables
int64_t system_current_time = 0;
int64_t last_bsec_timestamp = 0;

//"Ready to send" flags
uint8_t mic_rdy = 0, opt_rdy = 0, bme_rdy = 0;

//Data packet for the radio module
dataPacket packet = {0}
void setup(){
    // stop watchdog
    WDT_A_hold(WDT_A_BASE);

    set_clock_signals();
    init_system_timer(SYSTEM_TIMER_CLK_DIVIDER);//timer overflows in 65ms (with 12MHz SMCLK)
    init_gpio();
    init_mic();
    init_spi(SLAVE); //sensing module set to slave in the radio<->sensing SPI connection
                     //BME680 is always a slave
    init_i2c();
    _enable_interrupt();
    opt3001_default_init();
    bsec_init_ret_val = bsec_iot_init(BSEC_SAMPLE_RATE_LP, 0.0f, bus_write, bus_read, sleep, state_load, config_load);
}

void send_data_to_radio_module(){
    if(mic_rdy){
        //add value to data packet
    }
    if(opt_rdy){
        //add value to data packet
    }
    if(bme_rdy){
        //add value to data packet
    }
    mic_rdy = 0;
    opt_rdy = 0;
    bme_rdy = 0;
}

void main (void)
{
    setup();
//    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    while(1){
        if(get_mic_status() == DONE){
            micResult = get_mic_result();
            mic_rdy = 1;
        }
//        __delay_cycles(12000);
//        system_current_time = get_system_time_ms();
//        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
        if(opt3001_get_status(CONVERSION_READY)){
//        if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == GPIO_INPUT_PIN_LOW){
//            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
            luxResult = opt3001_get_lux_result();
            opt_rdy = 1;
        }
//        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
//        __delay_cycles(12000000);
//        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
        bsec_iot_service(sleep, get_timestamp_us, output_ready, state_save, 10000);
        if(my_bsec_output.timestamp > last_bsec_timestamp){
            last_bsec_timestamp = my_bsec_output.timestamp;
            bme_rdy = 1;
        }
//        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);

        send_data_to_radio_module();
//        __delay_cycles(6000000);
    }

}

//******************************************************************************
//
//This is the EUSCI_A1_SPI interrupt vector service routine. (radio module SPI)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void USCI_A1_ISR(void)
{
    switch(__even_in_range(UCA1IV, USCI_SPI_UCTXIFG))
    {
        case USCI_SPI_UCRXIFG:      // UCRXIFG
//            //USCI_A0 TX buffer ready?
//            while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
//                        EUSCI_A_SPI_TRANSMIT_INTERRUPT));
//
//            RXData = EUSCI_A_SPI_receiveData(EUSCI_A0_BASE);
//
//            //Increment data
//            TXData++;
//
//            //Send next value
//            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE,
//                    TXData
//                    );
//
//            //Delay between transmissions for slave to process information
//            __delay_cycles(40);
            break;
        default:
            break;
    }
}
