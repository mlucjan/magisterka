#include <stdlib.h>
#include "mcu_config/clock_system.h"
#include "sensors/mic.h"
#include "mcu_config/peripherals.h"
#include "sensors/opt3001.h"

unsigned char timerRunning = 0x00; //mic timer running flag
uint32_t currResult;
uint32_t maxResult = 0;
uint32_t minResult = 0x00FFFFFF;
uint16_t timerOverflowCounter = 0;
uint16_t maxTimerOverflows = 10; //12MHz clock + divider(2) -> timer overflows in ~11ms

void setup(){
    // stop watchdog
    WDT_A_hold(WDT_A_BASE);

    set_clock_signals();
    init_gpio();
    init_sdc();
    init_spi(SLAVE); //sensing module is a slave in the radio<->sensing SPI connection
    init_i2c();

    //Start timer in continuous mode sourced by SMCLK
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_2;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;

    //start mic measurement
    timerRunning = 0x01;
    Timer_A_initContinuousMode(TIMER_A1_BASE, &param);
    SD24_B_startConverterConversion(SD24_BASE, 0);
}

void main (void)
{
    setup();

    while(1){
        if(timerRunning == 0x00){

            SD24_B_stopConverterConversion(SD24_BASE, 0);

            //TODO: save maxResult and minResult somewhere else or send them to the radio module

            timerRunning = 0x01;
            minResult = 0x00FFFFFF;
            maxResult = 0;
            Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
            SD24_B_startConverterConversion(SD24_BASE, 0);
        }

    }

}

//******************************************************************************
//
//This is the TIMER1_A3 interrupt vector service routine. (mic timer)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER1_A1_VECTOR)))
#endif
void TIMER1_A1_ISR (void)
{
    //Any access, read or write, of the TAIV register automatically resets the
    //highest "pending" interrupt flag
    switch ( __even_in_range(TA1IV,14) ){
        case  0: break;                          //No interrupt
        case  2: break;                          //CCR1 not used
        case  4: break;                          //CCR2 not used
        case  6: break;                          //CCR3 not used
        case  8: break;                          //CCR4 not used
        case 10: break;                          //CCR5 not used
        case 12: break;                          //CCR6 not used
        case 14:
                                                    // overflow
            timerOverflowCounter++;
            if(timerOverflowCounter >= maxTimerOverflows){ // timer do odmierzania długości cyklu próbkowania
                timerOverflowCounter = 0;
                timerRunning = 0x00;
                Timer_A_stop(TIMER_A1_BASE);
                Timer_A_clear(TIMER_A1_BASE);
            }
            break;
        default: break;
    }
}
//******************************************************************************
//
//This is the SD24_B interrupt vector service routine. (mic sigma-delta converter)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD24B_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SD24B_VECTOR)))
#endif
void SD24BISR(void)
{
    switch (SD24BIV)
    {
        case SD24BIV_SD24OVIFG:             // SD24MEM Overflow
            break;
        case SD24BIV_SD24TRGIFG:            // SD24 Trigger IFG
            break;
        case SD24BIV_SD24IFG0:              // SD24MEM0 IFG
            // Save CH0 results (clears IFG)
            currResult = SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_0);
            if(currResult > maxResult){ //śledzenie wartości max i min
                maxResult = currResult;
            }
            else if(currResult < minResult){
                minResult = currResult;
            }
            break;
        case SD24BIV_SD24IFG1:              // SD24MEM1 IFG
            break;
        case SD24BIV_SD24IFG2:              // SD24MEM2 IFG
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
