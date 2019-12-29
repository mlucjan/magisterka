/*
 * bsec_msp430_implementation.c
 *
 *  Created on: 12 lis 2019
 *      Author: michal
 */
#include "bsec_msp430_implementation.h"

///Tymczasowy workaround na timestampa
//int64_t system_current_time = 0;
extern int64_t systemTimerOverflows;

struct bsec_output my_bsec_output;

/*!
 * @brief           Write operation in either I2C or SPI
 *
 * param[in]        dev_addr        I2C or SPI device address
 * param[in]        reg_addr        register address
 * param[in]        reg_data_ptr    pointer to the data to be written
 * param[in]        data_len        number of bytes to be written
 *
 * @return          result of the bus communication function
 */
int8_t bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{
    // ...
    // Please insert system specific function to write to the bus where BME680 is connected
    // ...

    int i,timeout;
    //Set register address to start writing to
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    EUSCI_A_SPI_transmitData(EUSCI_A2_BASE, reg_addr);
    for(timeout=0; timeout < 10; timeout++){
        if(!EUSCI_A_SPI_isBusy(EUSCI_A2_BASE)) break;
        __delay_cycles(120);
    }
    //Transmit data
    for(i=0; i < data_len; i++){
        EUSCI_A_SPI_transmitData(EUSCI_A2_BASE, reg_data_ptr[i]);

        for(timeout=0; timeout < 10; timeout++){
            if(!EUSCI_A_SPI_isBusy(EUSCI_A2_BASE)) break;
            __delay_cycles(120);
        }
    }
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
    return 0;
}

/*!
 * @brief           Read operation in either I2C or SPI
 *
 * param[in]        dev_addr        I2C or SPI device address
 * param[in]        reg_addr        register address
 * param[out]       reg_data_ptr    pointer to the memory to be used to store the read data
 * param[in]        data_len        number of bytes to be read
 *
 * @return          result of the bus communication function
 */
int8_t bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data_ptr, uint16_t data_len)
{
    // ...
    // Please insert system specific function to read from bus where BME680 is connected
    // ...

    int i, j;
    //Set register address to start reading from
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    EUSCI_A_SPI_transmitData(EUSCI_A2_BASE, reg_addr);

    for(j=0; j < 10; j++){
        if(!EUSCI_A_SPI_isBusy(EUSCI_A2_BASE)) break;
        __delay_cycles(120);
    }

//    __delay_cycles(6000); //0,5ms delay
    //Transmit data
    for(i=0; i < data_len; i++){
        EUSCI_A_SPI_transmitData(EUSCI_A2_BASE, 0xff);

        for(j=0; j < 10; j++){
            if(!EUSCI_A_SPI_isBusy(EUSCI_A2_BASE)) break;
            __delay_cycles(120);
        }

        reg_data_ptr[i] = EUSCI_A_SPI_receiveData(EUSCI_A2_BASE);
    }
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
    return 0;
}

/*!
 * @brief           System specific implementation of sleep function
 *
 * @param[in]       t_ms    time in milliseconds
 *
 * @return          none
 */
void sleep(uint32_t t_ms)
{
    // ...
    // Please insert system specific function sleep or delay for t_ms milliseconds
    // ...
    while(t_ms--){
        __delay_cycles(UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ);
    }
}

/*!
 * @brief           Capture the system time in microseconds
 *
 * @return          system_current_time    current system timestamp in microseconds
 */
int64_t get_timestamp_us()
{
    // ...
    // Please insert system specific function to retrieve a timestamp (in microseconds)
    // ...
    return get_system_time_ms()*1000;
}

/*!
 * @brief           Handling of the ready outputs
 *
 * @param[in]       timestamp       time in nanoseconds
 * @param[in]       iaq             IAQ signal
 * @param[in]       iaq_accuracy    accuracy of IAQ signal
 * @param[in]       temperature     temperature signal
 * @param[in]       humidity        humidity signal
 * @param[in]       pressure        pressure signal
 * @param[in]       raw_temperature raw temperature signal
 * @param[in]       raw_humidity    raw humidity signal
 * @param[in]       gas             raw gas sensor signal
 * @param[in]       bsec_status     value returned by the bsec_do_steps() call
 *
 * @return          none
 */
void output_ready(int64_t timestamp, float iaq, uint8_t iaq_accuracy, float temperature, float humidity,
     float pressure, float raw_temperature, float raw_humidity, float gas, bsec_library_return_t bsec_status,
     float static_iaq, float co2_equivalent, float breath_voc_equivalent)
{
    my_bsec_output.timestamp = timestamp;
    my_bsec_output.iaq = iaq;
    my_bsec_output.iaq_accuracy = iaq_accuracy;
    my_bsec_output.temperature = temperature;
    my_bsec_output.humidity = humidity;
    my_bsec_output.pressure = pressure;
    my_bsec_output.raw_temperature = raw_temperature;
    my_bsec_output.humidity = raw_humidity;
    my_bsec_output.gas = gas;
    my_bsec_output.bsec_status = bsec_status;
    my_bsec_output.static_iaq = static_iaq;
    my_bsec_output.co2_equivalent = co2_equivalent;
    my_bsec_output.breath_voc_equivalent = breath_voc_equivalent;
}

/*!
 * @brief           Load previous library state from non-volatile memory
 *
 * @param[in,out]   state_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to state_buffer
 */
uint32_t state_load(uint8_t *state_buffer, uint32_t n_buffer)
{
    // ...
    // Load a previous library state from non-volatile memory, if available.
    //
    // Return zero if loading was unsuccessful or no state was available,
    // otherwise return length of loaded state string.
    // ...
    return 0;
}

/*!
 * @brief           Save library state to non-volatile memory
 *
 * @param[in]       state_buffer    buffer holding the state to be stored
 * @param[in]       length          length of the state string to be stored
 *
 * @return          none
 */
void state_save(const uint8_t *state_buffer, uint32_t length)
{
    // ...
    // Save the string some form of non-volatile memory, if possible.
    // ...
}

/*!
 * @brief           Load library config from non-volatile memory
 *
 * @param[in,out]   config_buffer    buffer to hold the loaded state string
 * @param[in]       n_buffer        size of the allocated state buffer
 *
 * @return          number of bytes copied to config_buffer
 */
uint32_t config_load(uint8_t *config_buffer, uint32_t n_buffer)
{
    // ...
    // Load a library config from non-volatile memory, if available.
    //
    // Return zero if loading was unsuccessful or no config was available,
    // otherwise return length of loaded config string.
    // ...

    //wczytywanie z bsec_serialized_configurations_iaq.h
    memcpy(config_buffer, bsec_config_iaq, sizeof(bsec_config_iaq));
    return sizeof(bsec_config_iaq);
}
