// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011
 
///////////////////////////////////////////////////////////////////////
// Filename: main.c
//
// Synopsis: Main program file for demonstration code
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h"
#include <time.h>
#include <stdio.h>

#define NUM_SAMPLES 10		// Change to 1024
#define NUM_AUTOCORR 5			// Change to 20

int main()
{    
	// initialize DSP board
  	DSP_Init();

	// call StartUp for application specific code
	// defined in each application directory
	StartUp();
	
	// main stalls here, interrupts drive operation 
  	while(1) {
  		;
  	}
}

void printArrays(volatile float * input, volatile float * output, time_t dur)
{
	int i;
	printf("\nx[n] = [ ");
	for (i = 0; i < NUM_SAMPLES; i++) {
		printf("%.2f ", input[i]);
	}
	printf("]\n");

	printf("R[k] = [ ");
	for (i = 0; i < NUM_AUTOCORR; i++) {
		printf("%.2f ", output[i]);
	}
	printf("]\n");

	printf("Processing time: %f ms\n", dur);

}

