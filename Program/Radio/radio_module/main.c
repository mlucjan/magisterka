#include "communication/sensing_module_comm.h"
#include "mcu_config/peripherals.h"
#include "communication/radio_comm.h"
#include "communication/data_packet.h"

//sensor data buffer
dataPacket packet;

//execution state flags
uint8_t pin = 0; //sensor data ready pin value
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
    init_radio();
    packet = packetInitValue;
    __delay_cycles(24000000);
}

void main (void)
{
    setup();
    while(1){
        pin = sensor_data_ready();
        if(pin){
            read_sensor_data(packet.bytearray, PACKET_SIZE_BYTES);
            if(packet.lux == 0) continue;
            send_data_radio(packet.bytearray, PACKET_SIZE_BYTES);
        }
    }

}
