#ifndef __BOARD_H__
#define __BOARD_H__

#define GPIO_PORT_LED           GPIO_PORT_P5
#define GPIO_PIN_ERR_LED        GPIO_PIN0
#define GPIO_PIN_OK_LED         GPIO_PIN1

#define GPIO_PORT_RADIO_SEL     GPIO_PORT_P3
#define GPIO_PIN_LORA_SEL       GPIO_PIN4 //dipswitch switch 2
#define GPIO_PIN_BLE_SEL        GPIO_PIN5 //dipswitch switch 1

#define GPIO_PORT_LORA_UART     GPIO_PORT_P1
#define GPIO_PIN_LORA_TX        GPIO_PIN7
#define GPIO_PIN_LORA_RX        GPIO_PIN6
#define GPIO_PIN_LORA_CTS       GPIO_PIN5
#define GPIO_PIN_LORA_RTS       GPIO_PIN4

#define GPIO_PORT_LORA_RESET    GPIO_PORT_P3
#define GPIO_PIN_LORA_RESET     GPIO_PIN7

#define GPIO_PORT_BLE_UART      GPIO_PORT_P4
#define GPIO_PIN_BLE_TX         GPIO_PIN3
#define GPIO_PIN_BLE_RX         GPIO_PIN2

#define GPIO_PORT_BLE_RESET     GPIO_PORT_P6
#define GPIO_PIN_BLE_RESET      GPIO_PIN0

#define GPIO_PORT_BQ            GPIO_PORT_P3
#define GPIO_PIN_BQ_STAT1       GPIO_PIN1
#define GPIO_PIN_BQ_STAT2       GPIO_PIN2
#define GPIO_PIN_BQ_PG          GPIO_PIN3

#define GPIO_PORT_SENS          GPIO_PORT_P1
#define GPIO_PIN_SENS_DATA_RDY  GPIO_PIN0
#define GPIO_PIN_SENS_CLK       GPIO_PIN1
#define GPIO_PIN_SENS_MOSI      GPIO_PIN2
#define GPIO_PIN_SENS_MISO      GPIO_PIN3

#define GPIO_PORT_EXT_SPI       GPIO_PORT_P4
#define GPIO_PIN_EXT_SPI_STE    GPIO_PIN4
#define GPIO_PIN_EXT_SPI_CLK    GPIO_PIN5
#define GPIO_PIN_EXT_SPI_MOSI   GPIO_PIN6
#define GPIO_PIN_EXT_SPI_MISO   GPIO_PIN7

#endif // __BOARD_H__

