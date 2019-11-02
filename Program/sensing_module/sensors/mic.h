#ifndef MIC_H_
#define MIC_H_

#include "driverlib.h"

typedef struct micMeasResult{
    uint32_t currResult;
    uint32_t maxResult;
    uint32_t minResult;
};


void init_mic();
uint32_t mic_convert_once();

#endif /* MIC_H_ */
