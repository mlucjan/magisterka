#ifndef CLOCK_SYSTEM_H_
#define CLOCK_SYSTEM_H_

#include "driverlib.h"

//Target frequency for MCLK in kHz
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   12000
//MCLK/FLLRef Ratio
#define UCS_MCLK_FLLREF_RATIO   366
#define SYSTEM_TIMER_CLK_DIVIDER TIMER_A_CLOCKSOURCE_DIVIDER_12

void set_clock_signals();
void init_system_timer(uint16_t clk_divider);
int64_t get_system_time_ms();

#endif /* CLOCK_SYSTEM_H_ */
