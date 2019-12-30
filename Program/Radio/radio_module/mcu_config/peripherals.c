/*
 * peripherals.c
 *
 *  Created on: 9 lip 2019
 *      Author: michal
 */
#include "peripherals.h"

void init_gpio(){
    //LED pins
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0 + GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0 + GPIO_PIN1);

    //BQ24070 status pins
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3);

    //radio module selection switch
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN4 + GPIO_PIN5);

    //LoRa reset
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);

    //BT reset
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0);

    //LoRa RTS/CTS
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4 + GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4 + GPIO_PIN5);

    //Sensing module SPI STE pin
    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN0);

    //BT RTS/CTS
//    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN3 + GPIO_PIN2);
//    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN3 + GPIO_PIN2);

}

void init_spi(){
    //set sensor module communication pins to SPI mode
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1,
            GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3,
            GPIO_PRIMARY_MODULE_FUNCTION
        );

    //Initialize sensor SPI
    EUSCI_B_SPI_initMasterParam sens_param = {0};
    sens_param.selectClockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK;
    sens_param.clockSourceFrequency = CS_getSMCLK();
    sens_param.desiredSpiClock = 500000;
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

    // Enable USCI_B0 RX interrupt
    EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE,
            EUSCI_B_SPI_RECEIVE_INTERRUPT
            );

    //set external communication pins to SPI mode
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P4,
            GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7,
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
        GPIO_PORT_P1,
        GPIO_PIN6 + GPIO_PIN7,
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
        GPIO_PORT_P4,
        GPIO_PIN2 + GPIO_PIN3,
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

        // Enable USCI_A0 RX interrupt
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
