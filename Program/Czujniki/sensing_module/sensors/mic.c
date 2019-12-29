#include "mic.h"
#include "mcu_config/peripherals.h"

micMeasResult measResult;
uint32_t currResult;
uint8_t micMeasReady = 0x00;
uint16_t timerOverflowCounter = 0;

void init_mic(){
    init_sdc();
    measResult.maxResult = 0;
    measResult.minResult = 0x00FFFFFF;

    //Start timer in continuous mode sourced by SMCLK
    Timer_A_clearTimerInterrupt(TIMER_A1_BASE);
    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_2;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;

    //start mic measurement
    Timer_A_initContinuousMode(TIMER_A1_BASE, &param);
    SD24_B_startConverterConversion(SD24_BASE, 0);

//    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
}

uint8_t get_mic_status(){
    if(micMeasReady){
        return DONE;
    }
    else return RUNNING;
}

micMeasResult get_mic_result(){
    micMeasResult returnValue = measResult;
    micMeasReady = 0x00;
    measResult.maxResult = 0;
    measResult.minResult = 0x00FFFFFF;
//    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_CONTINUOUS_MODE);
    SD24_B_startConverterConversion(SD24_BASE, 0);
    return returnValue;
}

//******************************************************************************
//
//This is the TIMER1_A1 interrupt vector service routine. (mic timer)
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
            timerOverflowCounter++; //odmierzanie czasu trwania cyklu probkowania
            if(timerOverflowCounter >= MAX_MIC_TIMER_OVERFLOWS){ //zatrzymaj, jesli przekroczyl zadana wartosc
//                GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
                SD24_B_stopConverterConversion(SD24_BASE, 0);
                timerOverflowCounter = 0;
                micMeasReady = 0x01;
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
            if(currResult > measResult.maxResult){ //śledzenie wartości max i min
                measResult.maxResult = currResult;
            }
            else if(currResult < measResult.minResult){
                measResult.minResult = currResult;
            }
            break;
        case SD24BIV_SD24IFG1:              // SD24MEM1 IFG
            break;
        case SD24BIV_SD24IFG2:              // SD24MEM2 IFG
            break;
        default: break;
    }
}
