// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011
 
///////////////////////////////////////////////////////////////////////
// Filename: main.c
//
// Synopsis: Main program file for demonstration code
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h"
#include <stdio.h>

#define NUM_SAMPLES 512

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

void printArrays(volatile float * input, volatile float * output)
{
	int i = 0; // NOTE: this index writes over input[0]; likely exceeded memory
	printf("\nx[n] = [ ");
	for (i = 0; i < NUM_SAMPLES; i++) {
		printf("%.2f ", input[i]);
	}
	printf("]\n");

	printf("x^[n] = [ ");
	for (i = 0; i < NUM_SAMPLES; i++) {
		printf("%.2f ", output[i]);
	}
	printf("]\n");
}

