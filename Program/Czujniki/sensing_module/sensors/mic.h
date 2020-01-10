#ifndef MIC_H_
#define MIC_H_

#include "driverlib.h"

typedef struct{
    uint32_t maxResult;
    uint32_t minResult;
}micMeasResult;

enum measurementState{
    RUNNING,
    DONE
};

#define MAX_MIC_TIMER_OVERFLOWS 200 //12MHz clock + divider(2) -> timer overflows in ~11ms -> measurement cycle = 2,2s

void init_mic();
uint8_t get_mic_status();
micMeasResult get_mic_result();

#endif /* MIC_H_ */
