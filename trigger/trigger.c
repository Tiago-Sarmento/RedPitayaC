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
#include "mqtt_test.h"
#include "fpga_osc.h"

//Buffer depth 
const int BUF = 16*1024;
const int N = 5000; 			// desired length of trace (1,..., 16383)
const int decimation = 64; 	// decimation: [1;8;64;1024;8192;65536]



int main(void) 
{
	mqtt_connect();
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
	int trace_counts; 		// counts how many traces were sampled in for loop
	int trig_ptr; 			// trigger pointer shows memory adress where trigger was met
	int trig_test;			// trigger test checks if writing the trace has completed yet
	int trigger_voltage= 0; // enter trigger voltage in [V] as parameter [1V...~600 RP units]
	g_osc_fpga_reg_mem->cha_thr = osc_fpga_cnv_v_to_cnt(trigger_voltage); //sets trigger voltage

	/***************************/
	/** MAIN ACQUISITION LOOP **/
	/***************************/
	for(twosecondloopes=0; twosecondloops<5;twosecondloops++)
	{
		for (trace_counts=0; trace_counts<100; trace_counts++)
		{
			/*Set trigger, begin acquisition when condition is met*/
			osc_fpga_arm_trigger(); //start acquiring, incrementing write pointer
			osc_fpga_set_trigger(0x6); // where do you want your triggering from?
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


			/*Wait for the acquisition to finish = trigger is set to 0*/
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

				if (cha_signal[ptr]>=8192){ // properly display negative values fix
					//        		printf("%d ",cha_signal[ptr]-16384);
					//fprintf(fp, "%d, ", cha_signal[ptr]-16384);

				}
				else{
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
			if (counterA < 3000){
				printf("failedA pulse\n");
			} else{ printf("goodA pulse\n"); }
			if (counterB < 3000){
				printf("failedB pulse\n");
			} else{ printf("goodB pulse\n");}
			printf("counterA = %i\n",counterA);
			//char *payloadMain[];
			//sprintf(payloadMain,"{\"messageid\": 1345, \"value\": %d, \"timestamp\": %u}",counterA,(unsigned)time(NULL));
			sprintf(payload,"%d",counterA);

			printf("counterB = %i\n",counterB);
			fprintf(fp, "\n");
			printf("iteration = %i", trace_counts);
			printf("time is = %u\n", (unsigned)time(NULL));
		}
		mqtt_send(twosecondloops);
	}
		//mqtt_send();
	// cleaning up all nice like mommy taught me
	fclose(fp);
	osc_fpga_exit();
	mqtt_disconnect();
	return 0;
}
