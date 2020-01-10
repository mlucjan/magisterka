//#include "driverlib.h"
//#include "communication/sensing_module.h"
#include "mcu_config/peripherals.h"
#include "communication/rn2483.h"
#include "communication/bl652.h"
#include "communication/data_packet.h"
//#include <stdlib.h>
#include "Board.h"

//sensor data buffer
dataPacket packet;
uint8_t testArrayRx[10] = {0};

//execution state flags
uint8_t pin = 0; //sensor data ready pin value
uint8_t sensRequestedByteIdx = 0;
uint8_t dataReceived = 0; //data from sensing module received
uint8_t rxInProgress = 0; //rx from sensing module in progress
extern uint8_t BLEConnected; //BL652 is connected to a host
uint8_t previousRadioChoice,currentRadioChoice = 0; //radio interface selection


int i,j = 0;

int timeout = 0;
//uint8_t testArrayBLE[5] = {0x31, 0x32, 0x33, 0x34, 0x35};

void clear_buffer(uint8_t* buffer, uint8_t size){
    uint8_t index = 0;
    for(index = 0; index < size; index++){
        buffer[i] = 0;
    }
}
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
      EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, 0xff);
      for(timeout = 0; timeout < 100; timeout++){
          if(!EUSCI_B_SPI_isBusy(EUSCI_B0_BASE)) break;
          __delay_cycles(120);
      }
//              __delay_cycles(12000);
      packet_bytearray[i] = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
      if(i == packet_size_bytes - 1){
          rxInProgress = 0;
          dataReceived = 1;
      }
    }
}

void setup(){
    //Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set DCO FLL reference = REFO
    CS_initClockSignal(
            CS_FLLREF,
            CS_REFOCLK_SELECT,
            CS_CLOCK_DIVIDER_1
    );

    //Set DCO clock freq to 16MHz
    CS_initFLLSettle(
        16000,
        487
        );

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */
    PMM_unlockLPM5();
    init_gpio();
    init_spi();
    init_uart();
    init_lora();
    init_BLE();
    currentRadioChoice = read_dipswitch_setting();
    packet = packetInitValue;
}

void main (void)
{
    setup();
    while(1){
        pin = sensor_data_ready();
        if(pin && !rxInProgress){
            clear_buffer(testArrayRx, 10);

            rxInProgress = 1;
            read_sensor_data(packet.bytearray, PACKET_SIZE_BYTES);
        }

        if(dataReceived){
                    //send the packet through radio
                    dataReceived = 0;
//                    __delay_cycles(16000000);
                    //check which interface should be used
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
                        if(previousRadioChoice == BLE_SELECTED) lora_wake_up();
                        lora_transmit_data_p2p(packet.bytearray, (uint8_t)sizeof(packet), 1000, 1000);
                        break;
                    case BLE_SELECTED:
                        if(previousRadioChoice != BLE_SELECTED) lora_sleep();
                        if(BLEConnected) BLE_transmit_data(packet.bytearray, (uint8_t)sizeof(packet));
                        break;
                    case BOTH_SELECTED:
                        if(previousRadioChoice == BLE_SELECTED) lora_wake_up();
                        lora_transmit_data_p2p(packet.bytearray, (uint8_t)sizeof(packet), 1000, 1000);
                        if(BLEConnected) BLE_transmit_data(packet.bytearray, (uint8_t)sizeof(packet));
                        break;
                    default:
                        break;
                    }
                }

//        pin = sensor_data_ready();
//        if(pin && !rxInProgress){
//            clear_buffer(testArrayRx, 10);
//////            read_sensor_data(packet.bytearray, PACKET_SIZE_BYTES);
//            EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
//                                         EUSCI_B_SPI_RECEIVE_INTERRUPT);
//            EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
//                                      EUSCI_B_SPI_RECEIVE_INTERRUPT);
//            rxInProgress = 1;
//            EUSCI_A_SPI_transmitData(EUSCI_B0_BASE, sensRequestedByteNum);
//        }
    }

}

//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=USCI_B0_VECTOR
//__interrupt
//#elif defined(__GNUC__)
//__attribute__((interrupt(USCI_B0_VECTOR)))
//#endif
//void USCI_B0_ISR(void)
//{
//    switch(__even_in_range(UCB0IV, USCI_SPI_UCTXIFG))
//    {
//        case USCI_SPI_UCRXIFG:      // UCRXIFG
//            __disable_interrupt();
//            //USCI_A0 TX buffer ready?
//            while (!EUSCI_B_SPI_getInterruptStatus(EUSCI_B0_BASE,
//                        EUSCI_B_SPI_TRANSMIT_INTERRUPT));
//            testArrayRx[sensRequestedByteIdx] = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
//            if(sensRequestedByteIdx <= 4){
////            packet.bytearray[sensTxIndex] = EUSCI_B_SPI_receiveData(EUSCI_B0_BASE);
//                //request next byte
////                __delay_cycles(4000000);
//                EUSCI_B_SPI_transmitData(EUSCI_B0_BASE, sensRequestedByteIdx);
//            }
//
////            if(sensTxIndex == PACKET_SIZE_BYTES){
//            if(sensRequestedByteIdx == 4){
//                EUSCI_B_SPI_disableInterrupt(EUSCI_B0_BASE,
//                                             EUSCI_B_SPI_RECEIVE_INTERRUPT);
//                EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
//                                             EUSCI_B_SPI_RECEIVE_INTERRUPT);
//                pin = sensor_data_ready();
//                rxInProgress = 0;
//                dataReceived = 1;
//                sensRequestedByteIdx = 0;
//            }
//            sensRequestedByteIdx++;
//            __enable_interrupt();
//            break;
//        default:
//            break;
//    }
//}
