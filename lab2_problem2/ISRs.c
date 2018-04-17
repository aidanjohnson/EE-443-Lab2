// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
  
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

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

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
  		input[itr] = CodecDataIn.Channel[LEFT]; //itr;
  		output[itr] = 0;
	} else if (itr == NUM_SAMPLES) {
		stack(input, NUM_SAMPLES, bytes, output);
		printArrays(input, output);
  	}
  	itr++;

	WriteCodecData(0);		// send output data to port
}



