// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include <stdio.h>
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

#define NUM_SAMPLES 512

volatile float input[NUM_SAMPLES];
volatile float output[NUM_SAMPLES];
int bytes = sizeof(float);
int itr = 0;

//extern void stack(volatile float *in, int n, int b, volatile float *out);

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

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
	printf("\nx[n] = [");
	for (i = 0; i < NUM_SAMPLES; i++) {
		printf("%.2f ", input[i]);
	}
	printf("]\n");

	printf("x^[n] = [");
	for (i = 0; i < NUM_SAMPLES; i++) {
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

  	if (itr < NUM_SAMPLES) {
  		input[itr] = CodecDataIn.Channel[LEFT];
  		output[itr] = 0;
  		itr++;
	} else {
		stack(input, NUM_SAMPLES, bytes, output);
		printArrays();
		itr = 0;
  	}

	WriteCodecData(0);		// send output data to port
}



