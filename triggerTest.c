/* Red Pitaya C API example Acquiring a signal from a buffer
 * This application acquires a signal on a specific channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "redpitaya/rp.h"

int main(int argc, char **argv){

        /* Print error, if rp_Init() function failed */
        if(rp_Init() != RP_OK){
                fprintf(stderr, "Rp api init failed!\n");
        }

        /*LOOB BACK FROM OUTPUT 2 - ONLY FOR TESTING*/
        rp_GenReset();
        rp_GenFreq(RP_CH_1, 20000.0);
        rp_GenAmp(RP_CH_1, 1.0);
	rp_GenWaveform(RP_CH_1, RP_WAVEFORM_SINE);
        rp_GenOutEnable(RP_CH_1);


        uint32_t buff_size = 16384/2;
        float *buff = (float *)malloc(buff_size * sizeof(float));
	//int32_t trig_delay = 0;
	//int32_t error_num;
        rp_AcqReset();
        rp_AcqSetDecimation(RP_DEC_64);
        rp_AcqSetGain(RP_CH_1,RP_HIGH); 
	rp_AcqSetTriggerLevel(RP_CH_1, 0.3);

 //       if (rp_AcqSetTriggerDelay(buff_size/2) != RP_OK){
//		fprintf(stderr,"Failed trigger delay\n");
//	}
	
	int getDelay;
	rp_AcqGetTriggerDelay(&getDelay);
	printf("Result = %i\n",getDelay);
	
        rp_AcqStart();
	


        /* After acquisition is started some time delay is needed in order to acquire fresh s$
         Here we have used time delay of one second but you can calculate exact value takin$
        length and smaling rate*/

        sleep(1);

//	rp_AcqGetTriggerDelay(&getDelay);
//	printf("Result = %i\n",getDelay);


        rp_AcqSetTriggerSrc(RP_TRIG_SRC_EXT_NE);
	int countLarge;
	int iterations;
	iterations = 0;
	while(iterations < 3){
	        float *buff = (float *)malloc(buff_size * sizeof(float));

	        rp_acq_trig_state_t state = RP_TRIG_STATE_WAITING;

	        printf("iterations = %i\n",iterations);
		while(1){
			rp_AcqGetTriggerState(&state);
			if(state == RP_TRIG_STATE_TRIGGERED){
			break;
			}
		}

		rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
		int i;
		FILE *fp1=NULL;
		fp1=fopen("analogueData.txt","w");
		countLarge = 0;
		for(i = 0; i < buff_size; i++){
			//printf("%f\n", buff[i]);
			fprintf(fp1,"%f\n", buff[i]);
			if (buff[i] > 0.1){
				countLarge++;
			}
		}
		printf("result = %i\n",countLarge);
		iterations++;
	        printf("iterations = %i\n",iterations);
		//break;
	}
        printf("test\n");
        /* Releasing resources */
        free(buff);
        rp_Release();
        return 0;
}

