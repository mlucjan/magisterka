#include "driverlib.h"
#include "mcu_config/peripherals.h"
#include "communication/rn2483.h"
#include "communication/data_packet.h"
#include <stdlib.h>

dataPacket packet;
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

    init_uart();
    init_spi();
    lora_init();
    packet = packetInitValue;
}

void main (void)
{
    setup();
    while(1){
        lora_transmit_data_p2p(packet.bytearray, (uint8_t)sizeof(packet), 1000, 1000);
        __delay_cycles(16000000);
    }

}
