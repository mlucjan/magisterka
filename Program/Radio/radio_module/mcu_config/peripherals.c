/*
 * peripherals.c
 *
 *  Created on: 9 lip 2019
 *      Author: michal
 */
#include "peripherals.h"
//#include "driverlib.h"
#include "Board.h"

void init_gpio(){
    //LED pins
    GPIO_setAsOutputPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED + GPIO_PIN_OK_LED);
    GPIO_setOutputLowOnPin(GPIO_PORT_LED, GPIO_PIN_ERR_LED + GPIO_PIN_OK_LED);

    //BQ24070 status pins
    GPIO_setAsOutputPin(GPIO_PORT_BQ, GPIO_PIN_BQ_STAT1 + GPIO_PIN_BQ_STAT1 + GPIO_PIN_BQ_STAT1);
    GPIO_setOutputLowOnPin(GPIO_PORT_BQ, GPIO_PIN_BQ_STAT1 + GPIO_PIN_BQ_STAT1 + GPIO_PIN_BQ_STAT1);

    //radio module selection switch
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_RADIO_SEL, GPIO_PIN_LORA_SEL + GPIO_PIN_BLE_SEL);

    //LoRa reset
    GPIO_setAsOutputPin(GPIO_PORT_LORA_RESET, GPIO_PIN_LORA_RESET);
    GPIO_setOutputLowOnPin(GPIO_PORT_LORA_RESET, GPIO_PIN_LORA_RESET);

    //BT reset
    GPIO_setAsOutputPin(GPIO_PORT_BLE_RESET, GPIO_PIN_BLE_RESET);
    GPIO_setOutputHighOnPin(GPIO_PORT_BLE_RESET, GPIO_PIN_BLE_RESET);

    //LoRa RTS/CTS
    GPIO_setAsOutputPin(GPIO_PORT_LORA_UART, GPIO_PIN_LORA_CTS + GPIO_PIN_LORA_RTS);
    GPIO_setOutputLowOnPin(GPIO_PORT_LORA_UART, GPIO_PIN_LORA_CTS + GPIO_PIN_LORA_RTS);

    //Sensing module SPI STE pin
    GPIO_setAsInputPin(GPIO_PORT_SENS, GPIO_PIN_SENS_DATA_RDY);

    //BT RTS/CTS
//    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3 + GPIO_PIN2);
//    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3 + GPIO_PIN2);

}

void init_spi(){
    //set sensor module communication pins to SPI mode
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_SENS,
            GPIO_PIN_SENS_CLK + GPIO_PIN_SENS_MOSI + GPIO_PIN_SENS_MISO,
            GPIO_PRIMARY_MODULE_FUNCTION
        );

    //Initialize sensor SPI
    EUSCI_B_SPI_initMasterParam sens_param = {0};
    sens_param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    sens_param.clockSourceFrequency = CS_getSMCLK();
    sens_param.desiredSpiClock = 250000;
    sens_param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    sens_param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    sens_param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    sens_param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(EUSCI_B0_BASE, &sens_param);

    //Enable sensor SPI module
    EUSCI_B_SPI_enable(EUSCI_B0_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT
            );

//    // Enable USCI_B0 RX interrupt
//    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
//            EUSCI_B_SPI_RECEIVE_INTERRUPT
//            );

    //set external communication pins to SPI mode
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_EXT_SPI,
            GPIO_PIN_EXT_SPI_CLK + GPIO_PIN_EXT_SPI_MOSI + GPIO_PIN_EXT_SPI_MISO,
            GPIO_PRIMARY_MODULE_FUNCTION
        );

    //Initialize external SPI
    EUSCI_B_SPI_initMasterParam ext_param = {0};
    ext_param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    ext_param.clockSourceFrequency = CS_getSMCLK();
    ext_param.desiredSpiClock = 500000;
    ext_param.msbFirst = EUSCI_B_SPI_MSB_FIRST;
    ext_param.clockPhase = EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    ext_param.clockPolarity = EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    ext_param.spiMode = EUSCI_B_SPI_3PIN;
    EUSCI_B_SPI_initMaster(EUSCI_B1_BASE, &ext_param);

    //Enable external SPI module
    EUSCI_B_SPI_enable(EUSCI_B1_BASE);

    EUSCI_B_SPI_clearInterrupt(EUSCI_B1_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT
            );

    // Enable USCI_B1 RX interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B1_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT
            );


}

void init_lora_uart(){
    //LoRa UART
    //Configure UART pins
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_LORA_UART,
        GPIO_PIN_LORA_RX + GPIO_PIN_LORA_TX,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    //Configure UART module with 9600 baudrate
    //SMCLK=16MHz
    EUSCI_A_UART_initParam param = {0};
        param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
        param.clockPrescalar = 104;
        param.firstModReg = 2;
        param.secondModReg = 0xB6; //0xB6 -> 182
        param.parity = EUSCI_A_UART_NO_PARITY;
        param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
        param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
        param.uartMode = EUSCI_A_UART_MODE;
        param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

        if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
            return;
        }

        EUSCI_A_UART_enable(EUSCI_A0_BASE);

        EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
            EUSCI_A_UART_RECEIVE_INTERRUPT);

        // Enable USCI_A0 RX interrupt
        EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
            EUSCI_A_UART_RECEIVE_INTERRUPT);
}

void init_ble_uart(){
    //BLE UART
    //Configure UART pins
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_BLE_UART,
        GPIO_PIN_BLE_RX + GPIO_PIN_BLE_TX,
        GPIO_PRIMARY_MODULE_FUNCTION
    );

    //Configure UART module with 115200 baudrate
    //SMCLK=16MHz
    EUSCI_A_UART_initParam param = {0};
        param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
        param.clockPrescalar = 8;
        param.firstModReg = 10;
        param.secondModReg = 0xF7; //0xF7 -> 247
        param.parity = EUSCI_A_UART_NO_PARITY;
        param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
        param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
        param.uartMode = EUSCI_A_UART_MODE;
        param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

        if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param)) {
            return;
        }

        EUSCI_A_UART_enable(EUSCI_A1_BASE);

        EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE,
            EUSCI_A_UART_RECEIVE_INTERRUPT);

        // Enable USCI_A1 RX interrupt
        EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
            EUSCI_A_UART_RECEIVE_INTERRUPT);
}

void init_uart(){
    init_lora_uart();
    init_ble_uart();
}

uint8_t read_dipswitch_setting(){
    uint8_t return_val = 0x00;

    if(GPIO_getInputPinValue(GPIO_PORT_RADIO_SEL, GPIO_PIN_LORA_SEL) == GPIO_INPUT_PIN_LOW){
        return_val |= 0x10;
    }
    if(GPIO_getInputPinValue(GPIO_PORT_RADIO_SEL, GPIO_PIN_BLE_SEL) == GPIO_INPUT_PIN_LOW){
        return_val |= 0x01;
    }
    return return_val;
}
