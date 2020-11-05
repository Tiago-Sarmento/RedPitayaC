// taken from https://github.com/Grozomah/trigger and adapted to use as failed pulse counter

/**
 * $Id: trigger.c peter.ferjancic 2014/11/17 $
 *
 * @brief Red Pitaya triggered acquisition of multiple traces
 *
 * (c) Red Pitaya  http://www.redpitaya.com
 *
 * This part of code is written in C programming language.
 * Please visit http://en.wikipedia.org/wiki/C_(programming_language)
 * for more details on the language used herein.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stddef.h>
#include <sys/param.h>
#include <time.h>
#include "mqtt_test.h"
#include "fpga_osc.h"

//definde buffer properties
const int BUF = 16*1024;
const int N = 2000; 		// desired length of trace (1,..., 16383)
const int decimation = 64; 	// decimation: [1;8;64;1024;8192;65536] select from defined values, see fpga_osc.h, 1.953 MS/s, https://redpitaya.readthedocs.io/en/latest/appsFeatures/examples/acqRF-samp-and-dec.html#s-rate-and-dec

//base sample rate = 125 MS/s
//actual sample rate = base sample rate/decimation = 1.953 MS/s
//time between sample = 1/actual sample rate =  5.1203277e-7
// duration of trace = time between sample * number of samples = 5.1203277e-7 * N = 1024us (N = 2000)

int main(void) 
{
	printf("initiated\n");
	mqtt_connect(); // from mqtt_test.h
	char payload[100] = "trigger";
	printf("starting process");
	mqtt_send(payload);
	// initialization
	int start = osc_fpga_init(); 
	if(start) {
		printf("osc_fpga_init didn't work, retval = %d",start);
		return -1;
	}
	FILE * fp;
	fp = fopen ("file.txt", "w+"); // open the file output is stored
	int * cha_signal; 
	int * chb_signal;

	// set acquisition parameters
	osc_fpga_set_trigger_delay(N);
	g_osc_fpga_reg_mem->data_dec = decimation;

	// put sampling engine into reset
	osc_fpga_reset();

	// define initial parameters
	int twosecondloops; 		// 
	int trace_counts; 		// counts how many traces were sampled in for loop
	int trig_ptr; 			// trigger pointer shows memory adress where trigger was met
	int trig_test;			// trigger test checks if writing the trace has completed yet
	int trigger_voltage= 0; // enter trigger voltage in [V] as parameter [1V...~600 RP units]
	g_osc_fpga_reg_mem->cha_thr = osc_fpga_cnv_v_to_cnt(trigger_voltage); //sets trigger voltage

	printf("entering loop\n");
	/***************************/
	/** MAIN ACQUISITION LOOP **/
	/***************************/
	for(twosecondloops=0; twosecondloops<20;twosecondloops++)
	{
		printf("two sec\n");
		for (trace_counts=0; trace_counts<100; trace_counts++)
		{
			printf("trace count\n");
			/*Set trigger, begin acquisition when condition is met*/
			osc_fpga_arm_trigger(); //start acquiring, incrementing write pointer
			osc_fpga_set_trigger(0x2); // where do you want your triggering from?
			/*    0 - end of acquisition/no acquisition
			*     1 - trig immediately
			*     2 - ChA positive edge 
			*     3 - ChA negative edge
			*     4 - ChB positive edge 
			*     5 - ChB negative edge
			*     6 - External trigger 0
			*     7 - External trigger 1*/
			// Trigger always changes to 0 after acquisition is completed, write pointer stops incrementing
			//->fpga.osc.h l66


			trig_test=(g_osc_fpga_reg_mem->trig_source); // it gets the above trigger value
			// if acquistion is not yet completed it should return the number you set above and 0 otherwise
			while (trig_test!=0) // with this loop the program waits until the acquistion is completed before cont.
			{
				trig_test=(g_osc_fpga_reg_mem->trig_source);
			}
			//->fpga_osc.c l366


			trig_ptr = g_osc_fpga_reg_mem->wr_ptr_trigger; // get pointer to mem. adress where trigger was met
			//->fpga_osc.c l283
			osc_fpga_get_sig_ptr(&cha_signal, &chb_signal); 
			//->fpga_osc.c l378


			/*now read N samples from the trigger pointer location.*/
			int i;
			int ptr;
			int counterA = 0;
			int counterB = 0;
			for (i=0; i < N; i++) {
				ptr = (trig_ptr+i)%BUF;
				printf("cha value, %i", cha_signal[ptr]);
				if (cha_signal[ptr]>=8192){ // properly display negative values fix
					//        		printf("%d ",cha_signal[ptr]-16384);
					//fprintf(fp, "%d, ", cha_signal[ptr]-16384);

				}
				else{ // points with value below 8192 are negative
					//        		printf("%d ",cha_signal[ptr]);
					//fprintf(fp, "%d, ", cha_signal[ptr]);;
					if(cha_signal[ptr] > 500){
						counterA++;
					}
				}
				if (chb_signal[ptr]>=8192){ // properly display negative values fix
					//        		printf("%d ",cha_signal[ptr]-16384);
					//fprintf(fp, "%d\n", chb_signal[ptr]-16384);
				}
				else{
					//        		printf("%d ",cha_signal[ptr]);
					//fprintf(fp, "%d\n", chb_signal[ptr]);;
					if(chb_signal[ptr] > 500){
						counterB++;
					}
				}  

				}
			// pulse failure defined by these conditions
			if (counterA < 300){
				printf("failedA pulse\n");
			} else{ printf("goodA pulse\n"); }
			if (counterB < 300){
				printf("failedB pulse\n");
			} else{ printf("goodB pulse\n");}
			printf("counterA = %i\n",counterA);
			char payloadMain[100]; // allocate excessive memory to avoid memory problems
			// sprintf copies string into variable
			sprintf(payloadMain,"{\"messageid\": 1345, \"value\": %d, \"timestamp\": %u}",counterA,(unsigned)time(NULL));
			sprintf(payload,"%d",counterA);
			mqtt_send(payloadMain); // can run at 50Hz without interrupting script
			printf("counterB = %i\n",counterB);
			fprintf(fp, "\n");
			printf("iteration = %i", trace_counts);
			printf("time is = %u\n", (unsigned)time(NULL));
		}
		
	}
	// clean up
	fclose(fp);
	osc_fpga_exit();
	printf("intentionally disconnecting MQTT\n");
	mqtt_disconnect();
	return 0;
}
