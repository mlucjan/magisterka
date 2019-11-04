#include <stdlib.h>
#include "mcu_config/clock_system.h"
#include "sensors/mic.h"
#include "mcu_config/peripherals.h"
#include "sensors/opt3001.h"

//unsigned char timerRunning = 0x00; //mic timer running flag
uint16_t wynik;
uint8_t luxMeasReady = 0x00;
float luxResult = 0;
micMeasResult micResult;
void setup(){
    // stop watchdog
    WDT_A_hold(WDT_A_BASE);

    set_clock_signals();
    init_gpio();
    init_mic();
    init_spi(SLAVE); //sensing module is a slave in the radio<->sensing SPI connection
                                    //BME680 is always a slave
    init_i2c();

//    // Setup P2.0 UCB0SCL, P2.1 UCB0SDA
//    P2SEL |= BIT0 | BIT1;                             // Set P2.0,P2.1 to UCB0SCL, UCB0SDA
//
//    // Setup eUSCI_B0
//    UCB0CTLW0 |= UCSWRST;                             // Enable SW reset
//    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSSEL_2;         // I2C Master, use SMCLK
//
//    UCB0BRW_L = 120;                                   // fSCL = SMCLK/120 = ~100kHz
//    UCB0BRW_H = 0;
//    UCB0I2CSA = 0x48;                                 // Slave Address is 048h
//    UCB0CTLW0 &= ~UCSWRST;                            // Clear SW reset, resume operation
//    UCB0IE |= UCTXIE0;                                // Enable TX interrupt

    _enable_interrupt();

    while(1){
         wynik = opt3001_register_read(MAN_ID_REG_ADDRESS);
    }

//    opt3001_default_init(); //poki co nie dziala

}

void send_data_to_radio_module(){

}

void main (void)
{
    setup();

    while(1){
        if(get_mic_status() == DONE){
            micResult = get_mic_result();
        }
        if(luxMeasReady){
            luxResult = opt3001_get_lux_result();
            luxMeasReady = 0x00;
        }
        /*
         * Obsluga BME680
         * */
    }

}
//******************************************************************************
//
//This is the GPIO Port 1 interrupt vector service routine. (OPT3001 INT pin)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(PORT1_VECTOR)))
#endif
void PORT1_ISR (void)
{
    switch ( __even_in_range(P1IV,P1IV_P1IFG7) ){
        case P1IV_NONE: break;                          //No interrupt
        case P1IV_P1IFG1: break;                          //Port P1.1
        case P1IV_P1IFG0: break;                          //Port P1.0
        case P1IV_P1IFG2: break;                          //Port P1.2
        case P1IV_P1IFG3: break;                          //Port P1.3
        case P1IV_P1IFG4: break;                          //Port P1.4
        case P1IV_P1IFG5: break;                          //Port P1.5
        case P1IV_P1IFG6: break;                          //Port P1.6
        case P1IV_P1IFG7:                                 //Port P1.7
            luxMeasReady = 0x01;
            break;
        default: break;
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
//******************************************************************************
//
//This is the EUSCI_A2_SPI interrupt vector service routine. (BME680 SPI)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A2_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A2_VECTOR)))
#endif
void USCI_A2_ISR(void)
{
    switch(__even_in_range(UCA2IV, USCI_SPI_UCTXIFG))
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
