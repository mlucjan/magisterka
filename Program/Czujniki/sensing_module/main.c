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
extern struct bsec_output myBsecOutput;
return_values_init bsecInitRetVal;

//System variables and flags
int64_t systemCurrentTime = 0;
int64_t lastBsecTimestamp = 0;
uint8_t sendingPacket = 0;
uint8_t txIndex = 0;

//"Ready to send" flags
uint8_t micRdy = 0, optRdy = 0, bmeRdy = 0;

//Data packet for the radio module
dataPacket packet;

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
    bsecInitRetVal = bsec_iot_init(BSEC_SAMPLE_RATE_LP, 0.0f, bus_write, bus_read, sleep, state_load, config_load);
    packet = packetInitValue;
}

void send_data_to_radio_module(){
    //form the packet
    if(micRdy){
        packet.micMax = micResult.maxResult;
        packet.micMin = micResult.minResult;
    }
    if(optRdy){
        packet.lux = luxResult;
    }
    if(bmeRdy){
        packet.iaq = myBsecOutput.iaq;
        packet.temperature = myBsecOutput.temperature;
        packet.humidity = myBsecOutput.humidity;
        packet.pressure = myBsecOutput.pressure;
    }
    //clear measurement result flags
    micRdy = 0;
    optRdy = 0;
    bmeRdy = 0;
    sendingPacket = 1;
    //set STE pin for radio module to start SPI transaction
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
}

void main (void)
{
    setup();
    while(1){
        if(!sendingPacket){
            if(get_mic_status() == DONE){
                micResult = get_mic_result();
                micRdy = 1;
            }

            if(opt3001_get_status(CONVERSION_READY)){
//            if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == GPIO_INPUT_PIN_LOW){
                luxResult = opt3001_get_lux_result();
                optRdy = 1;
            }

            bsec_iot_service(sleep, get_timestamp_us, output_ready, state_save, 10000);
            if(myBsecOutput.timestamp > lastBsecTimestamp){
                lastBsecTimestamp = myBsecOutput.timestamp;
                bmeRdy = 1;
            }
            //mic data sent as frequently as possible
            if(micRdy){
                send_data_to_radio_module();
            }
        }
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
            //USCI_A0 TX buffer ready?
            while (!EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE,
                        EUSCI_A_SPI_TRANSMIT_INTERRUPT));
            //Send byte of the packet
            EUSCI_A_SPI_transmitData(EUSCI_A0_BASE, packet.bytearray[txIndex]);
            txIndex++;
            if(txIndex == PACKET_SIZE_BYTES){
                sendingPacket = 0;
                txIndex = 0;
                packet = packetInitValue;
                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
            }
            break;
        default:
            break;
    }
}
