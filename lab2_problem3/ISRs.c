// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include <time.h>
#include <stdio.h>

// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

#define NUM_SAMPLES 10		// Change to 1024
#define NUM_AUTOCORR 5

volatile float input[NUM_SAMPLES];
volatile float output[NUM_AUTOCORR];
volatile float coeff[NUM_AUTOCORR];
int bytes = sizeof(float);
int itr = 0;

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

void autocorrelation()
///////////////////////////////////////////////////////////////////////
// Purpose:   Computes the autocorrelation
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     None
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{
	int k, n;
	for (k = 0; k < NUM_AUTOCORR; k++) {
		for (n = 0; n < NUM_SAMPLES - k; n++) {
			output[k] = input[n]*input[n + k];
		}
	}
}

void printArrays()
///////////////////////////////////////////////////////////////////////
// Purpose:   Codec interface interrupt service routine
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     CheckForOverrun, ReadCodecData, WriteCodecData
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
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
}

interrupt void Codec_ISR()
///////////////////////////////////////////////////////////////////////
// Purpose:   Codec interface interrupt service routine  
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     CheckForOverrun, ReadCodecData, WriteCodecData
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{                    

 	if(CheckForOverrun())					// overrun error occurred (i.e. halted DSP)
		return;								// so serial port is reset to recover

  	CodecDataIn.UINT = ReadCodecData();		// get input data samples

  	time_t start, stop;

  	if (itr < NUM_SAMPLES) {
  		input[itr] = itr;//CodecDataIn.Channel[LEFT];
  		output[itr] = 0;
  		itr++;
	} else if (itr == NUM_SAMPLES) {
		start = time(0);
//		autocorrelation();
		xcorr(input, NUM_SAMPLES, NUM_AUTOCORR, output); //autocorrelated
		printArrays();
		itr++;
		stop = time(0);
		time_t dur = stop - start;
		printf("Processing time: %d \n", dur);
  	}


	WriteCodecData(0);		// send output data to port
}
