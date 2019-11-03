/**
 * Copyright (C) 2018 Bosch Sensortec GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * Neither the name of the copyright holder nor the names of the
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 * The information provided is believed to be accurate and reliable.
 * The copyright holder assumes no responsibility
 * for the consequences of use
 * of such information nor for any infringement of patents or
 * other rights of third parties which may result from its use.
 * No license is granted by implication or otherwise under any patent or
 * patent rights of the copyright holder.
 *
 * @file    bsec_iot_example.c
 * @date    Dec-10-2018
 * @version 1.1
 *
 */

/*!
 * @brief
 * Example for using of BSEC library in a fixed configuration with the BME680 sensor.
 * This works by running an endless loop in the bsec_iot_loop() function.
 */

/*!
 * @addtogroup bsec_examples BSEC Examples
 * @brief BSEC usage examples
 * @{*/

/**********************************************************************************************************************/
/* header files */
/**********************************************************************************************************************/

/*********************************************************************/
/* system header files */
#include "time.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <getopt.h>
#include "coines.h"

/*********************************************************************/
/* Own header files */
#include "bsec_integration_backend.h"

/*********************************************************************/
/* global variables */
char *outfile = NULL;
FILE *output_file;

/*********************************************************************/
/* function declarations */

/*! This internal API is used to handle ctrl+c signal */
static void ctrl_c_sig_handler(int sig_num);

/**********************************************************************************************************************/
/* functions */
/**********************************************************************************************************************/

/*!
 * @brief           System specific implementation of sleep function
 *
 * @param[in]       t_ms    time in milliseconds
 *
 * @return          none
 */
void sleep(uint32_t t_ms)
{
    /* System specific function sleep or delay for t_ms milliseconds */
    coines_delay_msec(t_ms);
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
    if (bsec_status == BSEC_OK)
    {
		/* Uncomment the printf below to display we sample output in your terminal. */
		/*
	    printf(	"Timestamp(ns): %"PRIu64", raw temp:%.3f, pres:%.3f, raw hum:%.3f, gas:%.3f, "
				"IAQ:%.3f, iaq_accuracy:%d, temp:%.3f, hum:%.3f, "
				"sIAQ:%.3f, CO2-eq:%.3f, bVOC-eq:%.3f\n", 	
				timestamp, 
				raw_temperature, 
				pressure, 
				raw_humidity,
				gas,
				iaq,  
				iaq_accuracy,
				temperature, 
				humidity, 
				static_iaq, 
				co2_equivalent, 
				breath_voc_equivalent);
        fflush(stdout);
		*/
		/* Open the output file */
		if (output_file) 
		{
			fprintf(output_file,	"%"PRIu64", %.3f, %.3f, %.3f, %.3f, %.3f, %d, %.3f, %.3f, %.3f, %.3f, %.3f\n", 	
									timestamp, 
									raw_temperature, 
									pressure, 
									raw_humidity,
									gas,
									iaq,  
									iaq_accuracy,
									temperature, 
									humidity, 
									static_iaq, 
									co2_equivalent, 
									breath_voc_equivalent);
		}
    }
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
    return 0;
}

/*!
 *  @brief This internal API is used to handle the CTRL+C signal
 *
 *  @param[in] sig_num
 *
 *  @return void
 *
 */
static void ctrl_c_sig_handler(int sig_num)
{
    /* Wait for 200ms */
    coines_delay_msec(200);

    /* Wait for 200ms */
    coines_delay_msec(200);
	
    /* Wait for 200ms */
    coines_delay_msec(200);
    exit(-1);
}

/*!
 *  @brief Main Function where the execution getting started to test the code.
 *
 *  @param[in] argc
 *  @param[in] argv
 *
 *  @return status
 *
 */
int main(int argc, char *argv[])
{
    return_values_init ret_bsec;
    int16_t rslt;
    char *infile = NULL;

    signal(SIGINT, ctrl_c_sig_handler);
	
	while ((rslt = getopt(argc, argv, "i:o:")) != -1)
    {
        switch (rslt) {
            case 'i':
                infile = optarg;
				printf("input file: %s\n", infile);
				fflush(stdout);
                break;
            case 'o':
                outfile = optarg;
				printf("output file: %s\n", outfile);
				fflush(stdout);
                break;
            default:
                return(0);
        }
    }

    /* Call to the function which initializes the BSEC library 
     * Switch on low-power mode and provide no temperature offset */
    ret_bsec = bsec_iot_init_backend(BSEC_SAMPLE_RATE_LP, 0.0f, state_load, config_load);
    if (ret_bsec.bme680_status)
    {
        /* Could not intialize BME680 */
        return (int)ret_bsec.bme680_status;
    }
    else if (ret_bsec.bsec_status)
    {
        /* Could not intialize BSEC library */
        return (int)ret_bsec.bsec_status;
    }
	
	/* Open the output file */
    if (outfile) 
    {
        output_file = fopen(outfile, "w");
        if (!output_file) 
        {
            perror("Unable to open output file");
            exit(2);
        }
		fprintf(output_file, "Timestamp(ns), raw temp, pres, raw hum, gas, IAQ, iaq_accuracy:, temp, hum, sIAQ, CO2-eq, bVOC-eq\n");
    }
	
	printf("BSEC initilization complete, now running BSEC...\r\n");
    fflush(stdout);

    /* Call a loop function which reads and processes data based on provided raw data file. */
    /* State is optionally saved every 10.000 samples, which means every 10.000 * 3 secs = 500 minutes */
	bsec_iot_loop_backend(BSEC_SAMPLE_RATE_LP, sleep, infile, output_ready, state_save, 10000);
	
	printf("BSEC processing complete!\r\n");
    fflush(stdout);

    if (output_file) 
    {
        fclose(output_file);
    }
    return 0;
}

/*! @}*/

