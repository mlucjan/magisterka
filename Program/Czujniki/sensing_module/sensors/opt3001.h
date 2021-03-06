/*
 * opt3001.h
 *
 *  Created on: 2 wrz 2019
 *      Author: michal
 */

#ifndef SENSORS_OPT3001_H_
#define SENSORS_OPT3001_H_

#include "driverlib.h"

//Register addresses-------------
#define RESULT_REG_ADDRESS 0x00
#define CONFIG_REG_ADDRESS 0x01
#define LOW_LIMIT_REG_ADDRESS 0x02
#define HIGH_LIMIT_REG_ADDRESS 0x03
#define MAN_ID_REG_ADDRESS 0x7e
#define DEV_ID_REG_ADDRESS 0x7f

//Transaction types---------------
#define REGISTER_READ 0
#define REGISTER_WRITE 1

//Configuration register field names
typedef enum {
    RANGE_NUMBER,
    CONVERSION_TIME,
    MODE_OF_CONVERSION,
    OVERFLOWN,
    CONVERSION_READY,
    FLAG_HIGH,
    FLAG_LOW,
    INT_POLARITY,
    MASK_EXPONENT,
    FAULT_COUNT,
}CONFIG_FIELD;

//value for error handling in opt3001_get_status()
#define GET_STATUS_FAIL 0xf

//Configuration field values-------
//Range number
#define AUTOMATIC_FULL_SCALE 0x0c
#define MANUAL_RANGE 0x0b
//Conversion time
#define CONV_TIME_100MS 0
#define CONV_TIME_800MS 1
//Mode of conversion operation
#define CONV_MODE_SHUTDOWN 0
#define CONV_MODE_SINGLE_SHOT 1
#define CONV_MODE_CONTINUOUS 2
//Latch mode
#define TRANSPARENT_HYSTERESIS_COMPARISON 0
#define LATCHED_WIDNOW_COMPARISON 1
//INT pin polarity
#define INT_ACTIVE_LOW 0
#define INT_ACTIVE_HIGH 1
//Fault count
#define FAULT_COUNT_1 0
#define FAULT_COUNT_2 1
#define FAULT_COUNT_4 2
#define FAULT_COUNT_8 4

//Configuration register structure-----------
typedef union {
    uint16_t value;
    struct  {
        //Little endian order
//      uint16_t rn : 4; //range number field (R/W)
//      uint16_t ct : 1; //conversion time field (R/W)
//      uint16_t m : 2; //mode of conversion operation field (R/W)
//      uint16_t ovf : 1; //overflow flag field (R)
//      uint16_t crf : 1; //conversion ready field (R)
//      uint16_t fh : 1; //flag high field (R)
//      uint16_t fl : 1; //flag low field (R)
//      uint16_t l : 1; //latch field (R/W)
//      uint16_t pol : 1; //INT pin polarity field (R/W)
//      uint16_t me : 1; //mask exponent field (R/W)
//      uint16_t fc : 2; //fault count field (R/W)
        //Big endian order
        uint16_t fc : 2; //fault count field (R/W)
        uint16_t me : 1; //mask exponent field (R/W)
        uint16_t pol : 1; //INT pin polarity field (R/W)
        uint16_t l : 1; //latch field (R/W)
        uint16_t fl : 1; //flag low field (R)
        uint16_t fh : 1; //flag high field (R)
        uint16_t crf : 1; //conversion ready field (R)
        uint16_t ovf : 1; //overflow flag field (R)
        uint16_t m : 2; //mode of conversion operation field (R/W)
        uint16_t ct : 1; //conversion time field (R/W)
        uint16_t rn : 4; //range number field (R/W)
    };
}configRegister;

//Result register structure-----------------
typedef union {
    uint16_t value;
    struct  {
      uint16_t fractResult : 12; //fractional result (R)
      uint16_t exponent : 4; //exponent (R)
    };
}resultRegister;

//Functions------------------------------------
uint16_t opt3001_register_read(uint8_t address);
void opt3001_register_write(uint8_t address, uint16_t data);
float opt3001_get_lux_result();
configRegister opt3001_get_config_register();
void opt3001_default_init();
void opt3001_custom_init(configRegister cr);
uint8_t opt3001_get_status(CONFIG_FIELD cf);

#endif /* SENSORS_OPT3001_H_ */
