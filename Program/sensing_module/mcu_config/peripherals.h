#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#include "driverlib.h"

typedef enum
{
    MASTER,
    SLAVE
}SPI_mode;

#define RADIO_SPI_CLK 500000 //desired SPI clock speed for communication with radio module
#define BME_SPI_CLK 500000 //desired SPI clock speed for communication with BME680
#define OPT3001_I2C_ADDRESS  0x44 //0b1000100 => 0x44
                                   //refer to init_gpio() in peripherals.c file

void init_gpio();
void init_sdc();
void init_spi();
void init_i2c();

#endif /* PERIPHERALS_H_ */
