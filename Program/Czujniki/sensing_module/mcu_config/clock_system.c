#include "clock_system.h"

int64_t systemTimerOverflows = 0;

void set_clock_signals(){

    //Set VCore = 1 for 12MHz clock
    PMM_setVCore(PMM_CORE_LEVEL_1);

    //Set DCO FLL reference = REFO
    UCS_initClockSignal(
            UCS_FLLREF,
            UCS_REFOCLK_SELECT,
            UCS_CLOCK_DIVIDER_1
    );

    //Set Ratio and Desired MCLK Frequency  and initialize DCO
    UCS_initFLLSettle(
            UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
            UCS_MCLK_FLLREF_RATIO
    );

}

void init_system_timer(uint16_t clk_divider){

    //Start timer in continuous mode sourced by SMCLK
    Timer_A_clearTimerInterrupt(TIMER_A3_BASE);
    Timer_A_initContinuousModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = clk_divider;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;

    Timer_A_initContinuousMode(TIMER_A3_BASE, &param);

}

int64_t get_system_time_ms(){
    uint16_t timerMaxValue = 65535;
    uint16_t timerCounterFreqKhz = UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ/SYSTEM_TIMER_CLK_DIVIDER;
    int16_t timerOverflowPeriod_ms = (int16_t)(timerMaxValue/timerCounterFreqKhz);
    return systemTimerOverflows * (int64_t)timerOverflowPeriod_ms + (int64_t)(Timer_A_getCounterValue(TIMER_A0_BASE)/timerCounterFreqKhz);
}

//******************************************************************************
//
//This is the TIMER1_A0 interrupt vector service routine. (system timer)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER3_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER3_A1_VECTOR)))
#endif
void TIMER1_A0_ISR (void)
{
    //Any access, read or write, of the TAIV register automatically resets the
    //highest "pending" interrupt flag
    switch ( __even_in_range(TA3IV,14) ){
        case  0: break;                          //No interrupt
        case  2: break;                          //CCR1 not used
        case  4: break;                          //CCR2 not used
        case  6: break;                          //CCR3 not used
        case  8: break;                          //CCR4 not used
        case 10: break;                          //CCR5 not used
        case 12: break;                          //CCR6 not used
        case 14:
            systemTimerOverflows++;
            break;
        default: break;
    }
}
