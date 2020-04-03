/**
 * $Id: trigger.c peter.ferjancic 2014/11/17 $
 * https://github.com/Grozomah/trigger/blob/master/trigger.c
 * @brief Red Pitaya triggered acquisition of multiple traces
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */
 
 /*******************************************************************************
 * Code added by the Red Pitaya bachelor thesis group:							*
 * + Read out of EEPROM calibration constants									*
 * + Argument passing															*
 * + Sending of data via TCP													*
 * + Infinite loop reading out data												*
 * + Minor structural adjustments												*
 ********************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <sys/param.h>
#include "fpga_osc.h"
#include "calib.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>

#define PORT 9930
	
void err(char *s)
{
    perror(s);
    exit(1);
}

//Buffer depth 
const int BUF = 16*1024;
const char *g_argv0 = NULL;
int counter=0;

rp_calib_params_t rp_calib_params;
/** Pointer to externally defined calibration parameters. */
rp_calib_params_t *gen_calib_params = NULL;

void usage() {

    const char *format =
        "\n"
		"Version: Hans-Kristian Yddal 2015-03-12\n"
		"Original version: trigger.c peter.ferjancic 2014/11/17\n"
        "Usage: %s   Trigger Samples Decimation Loops\n"
        "\n"
        "\tTrigger     0: End of acquisition/no acquisition\n"
		"\t            1: Trigger immediately\n"
		"\t            2: Channel A rising edge\n"
		"\t            3: Channel A falling edge\n"
		"\t            4: Channel B rising edge\n"
		"\t            5: Channel B falling edge\n"
		"\t            6: External trigger 0\n"
		"\t            7: External trigger 1\n"
		"\tSamples     Desired length of trace (1,..., 16383)\n"
        "\tDecimation  [1;8;64;1024;8192;65536]\n"
        "\tLoops       Amount of times to read signal (0 for infinite loop)\n"
        "\n";

    fprintf(stderr, format, g_argv0);
}

int main(int argc, char *argv[])  // argc er antall argument *argv[] er array med inndata
{
	g_argv0 = argv[0];
//	clock_t start_t, end_t, total_t;
	// argument parsing
	if ( argc < 4 ) {
        usage();
        return -1;
    }
	
	// Trigger argument parsing
	int trigger = atoi(argv[1]);
	if ( (trigger > 7) | (trigger < 0) ) {
		fprintf(stderr, "Invalid trigger argument: %s\n", argv[1]);
        usage();
        return -1;
	}
	
	// Samplesize argument parsing
	int sampleSize = atoi(argv[2]);
	if ((sampleSize < 1) | (sampleSize > 16384)){
		fprintf(stderr, "Invalid samples size: %s\n", argv[2]);
        usage();
        return -1;
	}
	
	// Decimation argument parsing
	int decimation = atoi(argv[3]);
	if ( (decimation == 1) | (decimation == 8) | (decimation == 64) | (decimation == 1024) | (decimation == 8192) | (decimation == 65536))
	{
	}
	else{
		fprintf(stderr, "Invalid decimation: %s\n", argv[3]);
        usage();
        return -1;
	}
	
	// Loop count argument parsing
	int loop = atoi(argv[4]);
	if (loop < 0)
	{
		fprintf(stderr, "Invalid loop count: %s\n", argv[3]);
        usage();
        return -1;
	}
	
	// Get calibration values from EEPROM
	rp_default_calib_params(&rp_calib_params);
    gen_calib_params = &rp_calib_params;

    if(rp_read_calib_params(gen_calib_params) < 0) {
        fprintf(stderr, "rp_read_calib_params() failed, using default"
            " parameters\n");
    }
	// use this to acquire calibrated offset: int offset = gen_calib_params->fe_ch1_dc_offs;

	// initialization
	int start = osc_fpga_init(); 
	if(start) 
	{
	    printf("osc_fpga_init didn't work, retval = %d",start);
	    return -1;
	}

	int * cha_signal;
	int * chb_signal;

	// set acquisition parameters
	osc_fpga_set_trigger_delay(sampleSize);
	g_osc_fpga_reg_mem->data_dec = decimation;

	// put sampling engine into reset
	osc_fpga_reset();
	
	// define initial parameters
	int trace_counts; 		// counts how many traces were sampled in for loop
	int trig_ptr; 			// trigger pointer shows memory adress where trigger was met
	int trig_test;			// trigger test checks if writing the trace has completed yet
	int trigger_voltage= 1; // enter trigger voltage in [V] as parameter [1V...~600 RP units]
	g_osc_fpga_reg_mem->chb_thr = osc_fpga_cnv_v_to_cnt(trigger_voltage); //sets trigger voltage
	
	// TCP init
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 
	connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); //Halts program until TCP connection is made

	trig_test=(g_osc_fpga_reg_mem->trig_source); // if acquistion is not yet completed it should return the number you set above and 0 otherwise
	if (loop > 0)
	{
		for (trace_counts=0; trace_counts<loop; trace_counts++)
		{
			osc_fpga_arm_trigger();
			osc_fpga_set_trigger(trigger); 
			trig_test=(g_osc_fpga_reg_mem->trig_source); // if acquistion is not yet completed it should return the number you set above and 0 otherwise
			while (trig_test!=0) // with this loop the program waits until the acquistion is completed before continue.
			{
				trig_test=(g_osc_fpga_reg_mem->trig_source);
			}

			trig_ptr = g_osc_fpga_reg_mem->wr_ptr_trigger; // get pointer to mem. adress where trigger was met
			osc_fpga_get_sig_ptr(&cha_signal, &chb_signal); 

			if (trig_ptr > (BUF-sampleSize)) // Enter logic to transition from end to beginning of cha_signal buffer.
			{
				write(connfd, &cha_signal[trig_ptr], sizeof(int)*(BUF-trig_ptr));
				write(connfd, &cha_signal[0], sizeof(int)*(sampleSize-(BUF-trig_ptr)));
			}
			else // Enter simple logic to send sampleSize from trigger point
			{
				write(connfd, &cha_signal[trig_ptr], sizeof(int)*sampleSize);
			}
		}
	}
	else
	{
		while (1) 
		{
			osc_fpga_arm_trigger();
			osc_fpga_set_trigger(trigger); 
			trig_test=(g_osc_fpga_reg_mem->trig_source); // if acquistion is not yet completed it should return the number you set above and 0 otherwise
			while (trig_test!=0) 
			{
				trig_test=(g_osc_fpga_reg_mem->trig_source);
			}

			trig_ptr = g_osc_fpga_reg_mem->wr_ptr_trigger; 
			osc_fpga_get_sig_ptr(&cha_signal, &chb_signal); 
			
			if (trig_ptr > (BUF-sampleSize)) // Enter logic to transition from end to beginning of cha_signal buffer.
			{
				write(connfd, &cha_signal[trig_ptr], sizeof(int)*(BUF-trig_ptr));
				write(connfd, &cha_signal[0], sizeof(int)*(sampleSize-(BUF-trig_ptr)));
			}
			else // Enter simple logic to send sampleSize from trigger point
			{
				write(connfd, &cha_signal[trig_ptr], sizeof(int)*sampleSize);
			}
		}	
	}

	// cleaning up all nice like mommy taught me
	close(connfd);
    osc_fpga_exit();
	return 0;
}
