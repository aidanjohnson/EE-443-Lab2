// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include "Echo.h"

// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

float xLeft, xRight, yLeft, yRight;
int idx = 0; 					// index for buffer value
#define MAX_DELAY 8000 			// maximum delay
float buffer[2][MAX_DELAY + 1]; // space for left + right
int delaySamples = 0; 			// number of samples to delay by


float getDelayedVals(int k)
///////////////////////////////////////////////////////////////////////
// Purpose:   Gets the delayed values for each channel
//
// Input:     None
//
// Returns:   Pointer to two floats
//
// Calls:     None
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{
	// Allocate memory for two floats
	float delayedVal;

	// For left channel, k = 0 and for right channel, k = 1
	int ind;	// Index of the delayed value in the buffer
	if (delaySamples > idx) {
		ind = MAX_DELAY - (delaySamples - idx);
	} else {
		ind = idx - delaySamples;
	}
	delayedVal = buffer[k][ind];

	return delayedVal;
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
	
	Int32 SW = ReadSwitches();

	// check switches: SW6 = 2^1, SW7 = 2^2; 0 = up, 1 = down
	// LEDs: D4 = 2^0, D5 = 2^1; 0 = off, 1 = on
	if(SW == 2) { // SW6 down, SW7 up: right out off, D4 off and D5 on
		delaySamples = 600;
		WriteLEDs(5);
	}
	else if(SW == 4) { // SW6 up, SW7 down: left out off, D4 on and D5 off
		delaySamples = 400;
		WriteLEDs(3);
	}
	else if(SW == 6) { // SW6 and SW7 down: right and left out off, D4 and D5 off
		delaySamples = 800;
		WriteLEDs(1);
	}
	else if(SW == 0) { // SW6 and SW7 up: right and left out on, D4 and D5 on
		delaySamples = 200;
		WriteLEDs(7);
	}
	else  {
		delaySamples = 0;
		WriteLEDs(0);
	}


	/****************************
	ECHO ROUTINE BEGINS HERE
	****************************/
	xLeft = CodecDataIn.Channel[LEFT];   // current LEFT input value to float
	xRight = CodecDataIn.Channel[RIGHT];   // current RIGHT input value to float

	buffer[LEFT][idx] = xLeft;
	buffer[RIGHT][idx] = xRight;
	if (++idx >= MAX_DELAY) // implement circular buffer
		idx = 0;

	// use either FIR or IIR lines below

	// for FIR comb filter effect, uncomment next two lines
	float leftDelay = getDelayedVals(LEFT);
	float rightDelay = getDelayedVals(RIGHT);
	if (delaySamples == 0) {
		yLeft = xLeft;
		yRight = xRight;
	}
	else {
		yLeft = xLeft + leftDelay;
		yRight = xRight + rightDelay;
	}

	CodecDataOut.Channel[LEFT] = yLeft;   // setup the LEFT value
	CodecDataOut.Channel[RIGHT] = yRight; // setup the RIGHT value
	/*****************************/

	WriteCodecData(CodecDataOut.UINT);		// send output data to  port
}
