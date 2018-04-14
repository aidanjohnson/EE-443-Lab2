// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011
 
///////////////////////////////////////////////////////////////////////
// Filename: main.c
//
// Synopsis: Main program file for demonstration code
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h"
#include <time.h> // time.h header

#define NUM_SAMPLES 10
#define K 20
volatile float input[NUM_SAMPLES];
volatile float output[K];
int itr = 0;

int main()
{    
	time_t start, stop; // defines start and stop times
	
	// initialize DSP board
  	DSP_Init();

  	start = time(0); // get current time (before starting the process)
  	wait(10000000); // do the processing

	// call StartUp for application specific code
	// defined in each application directory
	StartUp();
	
	// main stalls here, interrupts drive operation 
  	while(1) { 
		if (itr == NUM_SAMPLES) {
			stop = time(0); // get current time (after finishing the process)
			time_t dur = stop-start; // find duration
			printf("R(k) autocorrelation : %d\n", output);
			printf("Processing time used : %d s\n", dur); // print the result
		}
  	}   
}


