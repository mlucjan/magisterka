/*
 * opt3001.c
 *
 *  Created on: 2 wrz 2019
 *      Author: michal
 */
#include "opt3001.h"

uint8_t txStepCtr = 0, rxStepCtr = 0;
uint8_t txDataBytes[2]; //all transmitted data is 16 bits long
uint8_t lastTransactionType = 1, lastRegAddr = 0, currRegAddr;
uint8_t rxData[2]; //received data buffer

uint16_t opt3001_register_read(uint8_t reg_address){
    rxStepCtr = 0;
    currRegAddr = reg_address;
    if(lastTransactionType != REGISTER_READ || reg_address != lastRegAddr){
        lastTransactionType = REGISTER_READ;
        lastRegAddr = reg_address;
        EUSCI_B_I2C_masterSendSingleByte(EUSCI_B0_BASE, reg_address);
        while (EUSCI_B_I2C_SENDING_STOP == EUSCI_B_I2C_masterIsStopSent(EUSCI_B0_BASE)){
            __delay_cycles(100);
        }
        __delay_cycles(1000);
    }
    EUSCI_B_I2C_masterReceiveStart(EUSCI_B0_BASE);
//    while(rxStepCtr != 2);
    int i = 0;
    for(i=0; i<10; i++){
        if(rxStepCtr == 2){
            return (uint16_t)rxData[0]<<8 | rxData[1];
        }
        __delay_cycles(12000);
    }
    return 0;

//    return (uint16_t)rxData[0]<<8 | rxData[1];
}

void opt3001_register_write(uint8_t reg_address, uint16_t data){
    lastTransactionType = REGISTER_WRITE;
    lastRegAddr = reg_address;
    txStepCtr = 0;
    txDataBytes[0] = (uint8_t)(data >> 8); //MSB byte
    txDataBytes[1] = (uint8_t)data; //LSB byte
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B0_BASE, reg_address);
}

float opt3001_get_lux_result(){
    resultRegister result = { .value = {opt3001_register_read(RESULT_REG_ADDRESS)} };
    return 0.01 * (float)(1 << result.exponent) * (float)result.fractResult; //formula from the datasheet - lux = 0.01 * 2^(exponent) * fract
}

configRegister opt3001_get_config_register(){
//    configRegister cr = {0};
//    cr.value = opt3001_register_read(CONFIG_REG_ADDRESS);
    configRegister cr = { .value = {opt3001_register_read(CONFIG_REG_ADDRESS)} };
    return cr;
}

void opt3001_default_init(){
    configRegister cr = opt3001_get_config_register();
    cr.rn = AUTOMATIC_FULL_SCALE;
    cr.ct = CONV_TIME_800MS;
    cr.m = CONV_MODE_CONTINUOUS;
    cr.l = LATCHED_WIDNOW_COMPARISON;
    cr.pol = INT_ACTIVE_LOW;
    cr.fc = FAULT_COUNT_4;
    opt3001_register_write(CONFIG_REG_ADDRESS, cr.value);
}

void opt3001_custom_init(configRegister cr){
    opt3001_register_write(CONFIG_REG_ADDRESS, cr.value);
    if(cr.pol == INT_ACTIVE_HIGH){
        GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN7, GPIO_LOW_TO_HIGH_TRANSITION);
    }
    else{
        GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
    }
}

uint8_t opt3001_get_status(CONFIG_FIELD cf){
    configRegister cr = opt3001_get_config_register();
    switch(cf){
        case RANGE_NUMBER:
            return (uint8_t)cr.rn;
        case CONVERSION_TIME:
            return (uint8_t)cr.ct;
        case MODE_OF_CONVERSION:
            return (uint8_t) cr.m;
        case OVERFLOWN:
            return (uint8_t)cr.ovf;
        case CONVERSION_READY:
            return (uint8_t)cr.crf;
        case FLAG_HIGH:
            return (uint8_t)cr.fh;
        case FLAG_LOW:
            return (uint8_t)cr.fl;
        case INT_POLARITY:
            return (uint8_t)cr.pol;
        case MASK_EXPONENT:
            return (uint8_t)cr.me;
        case FAULT_COUNT:
            return (uint8_t)cr.fc;
        default:
            return GET_STATUS_FAIL;
    }
}

//******************************************************************************
//
//This is the EUSCI_B_I2C interrupt vector service routine. (OPT3001 I2C)
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
    switch(__even_in_range(UCB0IV, USCI_I2C_UCBIT9IFG))
    {
        case USCI_NONE:             // No interrupts break;
            break;
        case USCI_I2C_UCALIFG:      // Arbitration lost
            break;
        case USCI_I2C_UCNACKIFG:    // NAK received (master only)
            //resend start if NACK
            EUSCI_B_I2C_masterSendStart(EUSCI_B0_BASE);
            break;
        case USCI_I2C_UCSTTIFG:     // START condition detected with own address (slave mode only)
            break;
        case USCI_I2C_UCSTPIFG:     // STOP condition detected (master & slave mode)
            break;
        case USCI_I2C_UCRXIFG3:     // RXIFG3
            break;
        case USCI_I2C_UCTXIFG3:     // TXIFG3
            break;
        case USCI_I2C_UCRXIFG2:     // RXIFG2
            break;
        case USCI_I2C_UCTXIFG2:     // TXIFG2
            break;
        case USCI_I2C_UCRXIFG1:     // RXIFG1
            break;
        case USCI_I2C_UCTXIFG1:     // TXIFG1
            break;

        case USCI_I2C_UCRXIFG0:     // RXIFG0
            rxData[rxStepCtr] = EUSCI_B_I2C_masterReceiveMultiByteNext(EUSCI_B0_BASE);
            rxStepCtr++;
            if(rxStepCtr == 2) EUSCI_B_I2C_masterReceiveMultiByteStop(EUSCI_B0_BASE);

//            if(rxStepCtr == 0){
//                rxData[0] = EUSCI_B_I2C_masterReceiveMultiByteNext(EUSCI_B0_BASE);
//                rxStepCtr = 1;
//            }
//            else if(rxStepCtr == 1){
//                EUSCI_B_I2C_masterSendMultiByteFinish(EUSCI_B0_BASE, (uint_8t)txDataByte); //transmit LSB byte and STOP
//                rxStepCtr = 2;
//            }
            break;

        case USCI_I2C_UCTXIFG0:     // TXIFG0
            if(txStepCtr == 0){
                EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B0_BASE, txDataBytes[txStepCtr]); //transmit MSB byte of data
            }
            else if(txStepCtr == 1){
                EUSCI_B_I2C_masterSendMultiByteFinish(EUSCI_B0_BASE, txDataBytes[txStepCtr]); //transmit LSB byte of data and STOP
            }
            txStepCtr++;
            break;
        case USCI_I2C_UCBCNTIFG:    // Byte count limit reached (UCBxTBCNT)
            break;
        case USCI_I2C_UCCLTOIFG:    // Clock low timeout - clock held low too long
            break;
        case USCI_I2C_UCBIT9IFG:    // Generated on 9th bit of a transmit (for debugging)
            break;
        default:
            break;
    }
}
