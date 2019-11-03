#include "peripherals.h"

void init_gpio(){

    //OPT3001 ADDR pin
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6); //sets OPT3001 I2C address to 0b1000101

    //OPT3001 INT pin
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN7); //to receive interrupts from OPT3001
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);

    //BME680 SPI CSB pin
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);

    //Radio module SPI STE pin
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);

}

void init_sdc(){

    SD24_B_initParam initParam = {0};
    initParam.clockSourceSelect = SD24_B_CLOCKSOURCE_SMCLK;
    initParam.clockPreDivider = SD24_B_PRECLOCKDIVIDER_1;
    initParam.clockDivider = SD24_B_CLOCKDIVIDER_6;
    initParam.referenceSelect = SD24_B_REF_INTERNAL;
    SD24_B_init(SD24_BASE, &initParam);

    SD24_B_setInterruptDelay(SD24_BASE,
            SD24_B_CONVERTER_0,
            SD24_B_FIRST_SAMPLE_INTERRUPT);

    SD24_B_initConverterParam initConverterParam = {0};
    initConverterParam.converter = SD24_B_CONVERTER_0;
    initConverterParam.alignment = SD24_B_ALIGN_RIGHT;
    initConverterParam.startSelect = SD24_B_CONVERSION_SELECT_SD24SC;
    initConverterParam.conversionMode = SD24_B_CONTINUOUS_MODE;
    SD24_B_initConverter(SD24_BASE, &initConverterParam);

    // Enable channel 0 interrupt
       SD24_B_clearInterrupt(SD24_BASE,
               SD24_B_CONVERTER_0,
               SD24_B_CONVERTER_INTERRUPT);
       SD24_B_enableInterrupt(SD24_BASE,
               SD24_B_CONVERTER_0,
               SD24_B_CONVERTER_INTERRUPT);

    __delay_cycles(0x3600);  // Delay for 1.5V REF startup

    SD24_B_setOversampling(SD24_BASE, SD24_B_CONVERTER_0, SD24_B_OVERSAMPLE_256);
    //SD24_B_setGain(SD24_BASE, SD24_B_CONVERTER_0, SD24_B_GAIN_1);

}

void init_radio_spi(radio_SPI_mode mode){

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                               GPIO_PIN4 + GPIO_PIN5);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
                                               GPIO_PIN4);

    if(mode == MASTER){
        //Initialize Master
        EUSCI_A_SPI_initMasterParam param = {0};
        param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
        param.clockSourceFrequency = UCS_getSMCLK();
        param.desiredSpiClock = RADIO_SPI_CLK;
        param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
        param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
        param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
        param.spiMode = EUSCI_A_SPI_3PIN;
        EUSCI_A_SPI_initMaster(EUSCI_A1_BASE, &param);

    }
    else if(mode == SLAVE){
        //Initialize slave to MSB first, inactive high clock polarity and 4 wire SPI
        EUSCI_A_SPI_initSlaveParam param = {0};
        param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
        param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
        param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
        param.spiMode = EUSCI_A_SPI_3PIN;
        EUSCI_A_SPI_initSlave(EUSCI_A1_BASE, &param);
    }

    //Enable SPI module
    EUSCI_A_SPI_enable(EUSCI_A1_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A1_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_A1 RX interrupt
    EUSCI_A_SPI_enableInterrupt(EUSCI_A1_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);
}

void init_bme680_spi(){
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
                                                   GPIO_PIN2 + GPIO_PIN3);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
                                                   GPIO_PIN5);
    //Initialize Master
    EUSCI_A_SPI_initMasterParam param = {0};
    param.selectClockSource = EUSCI_A_SPI_CLOCKSOURCE_SMCLK;
    param.clockSourceFrequency = UCS_getSMCLK();
    param.desiredSpiClock = BME_SPI_CLK;
    param.msbFirst = EUSCI_A_SPI_MSB_FIRST;
    param.clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT;
    param.clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_HIGH;
    param.spiMode = EUSCI_A_SPI_3PIN;
    EUSCI_A_SPI_initMaster(EUSCI_A2_BASE, &param);

    //Enable SPI module
    EUSCI_A_SPI_enable(EUSCI_A2_BASE);

    EUSCI_A_SPI_clearInterrupt(EUSCI_A2_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);
    // Enable USCI_A1 RX interrupt
    EUSCI_A_SPI_enableInterrupt(EUSCI_A2_BASE,
            EUSCI_A_SPI_RECEIVE_INTERRUPT);

}

void init_spi(radio_SPI_mode radio_spi_mode){

    init_radio_spi(radio_spi_mode);
    init_bme680_spi();

}

void init_i2c(){

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2,
                                               GPIO_PIN0 + GPIO_PIN1);

    EUSCI_B_I2C_initMasterParam param = {0};
    param.selectClockSource = EUSCI_B_I2C_CLOCKSOURCE_SMCLK;
    param.i2cClk = UCS_getSMCLK();
    param.dataRate = EUSCI_B_I2C_SET_DATA_RATE_100KBPS;
    param.byteCounterThreshold = 0;
    param.autoSTOPGeneration = EUSCI_B_I2C_NO_AUTO_STOP;
    EUSCI_B_I2C_initMaster(EUSCI_B0_BASE, &param);

    //Specify slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B0_BASE,
                                OPT3001_I2C_ADDRESS
            );

    //Set Master in transmit mode
    EUSCI_B_I2C_setMode(EUSCI_B0_BASE,
            EUSCI_B_I2C_TRANSMIT_MODE
            );

    //Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B0_BASE);

    EUSCI_B_I2C_clearInterrupt(EUSCI_B0_BASE,
                               EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                               EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                               EUSCI_B_I2C_NAK_INTERRUPT
                               );

    //Enable master Receive and Transmit interrupt
    EUSCI_B_I2C_enableInterrupt(EUSCI_B0_BASE,
                                EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                                EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                                EUSCI_B_I2C_NAK_INTERRUPT
                                );
}
