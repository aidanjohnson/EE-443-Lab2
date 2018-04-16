// Welch, Wright, & Morrow, 
// Real-time Digital Signal Processing, 2011

///////////////////////////////////////////////////////////////////////
// Filename: ISRs.c
//
// Synopsis: Interrupt service routine for codec data transmit/receive
//
///////////////////////////////////////////////////////////////////////

#include "DSP_Config.h" 
#include <stdlib.h>
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1
#define A 0								// Index for channel A
#define B 1								// Index for channel B
#define MAX_DELAY 700

int idx = 0;							// Index of the end of the buffer
float channDelay[2];
float channGain[2];
int channGainInc[2];					// 1 if channel gain is increasing, 0 if decreasing
int upwardPitch;						// 1 if upward pitch change, 0 if downward pitch change
int pitchShiftIsOn;						// 1 if pitch shifter is enabled, 0 is disabled
int prevSW = -1;						// Previous switch values
volatile float buffer[MAX_DELAY + 1];	// 0 to 700


volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;

void updateDelays()
///////////////////////////////////////////////////////////////////////
// Purpose:   Updates channel A and B delays
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
	int k;
	// For channel A, k = 0 and for channel B, k = 1
	for (k = 0; k < 2; k++) {
		if (upwardPitch) {
			if (channDelay[A] <= 0) {
				channDelay[A] = (float)MAX_DELAY;
			} else {
				channDelay[A] -= 0.5;
			}
		} else {
			if (channDelay[A] >= 1) {
				channDelay[A] = (float)MAX_DELAY;
			} else {
				channDelay[A] += 0.5;
			}
		}
	}
}

void updateGains()
///////////////////////////////////////////////////////////////////////
// Purpose:   Updates channel A and B gains and increase/decrease flags
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
	// For channel A, k = 0 and for channel B, k = 1
	int k;
	for (k = 0; k < 2; k++) {
		if (channGainInc[k]) {
			if (channGain[k] >= 1) {
				channGainInc[k] = 0;
				channGain[k] -= (float)1/MAX_DELAY;
			} else {
				channGain[k] += (float)1/MAX_DELAY;
			}
		} else {
			if (channGain[k] <= 0) {
				channGainInc[k] = 1;
				channGain[k] += (float)1/MAX_DELAY;
			} else {
				channGain[k] -= (float)1/MAX_DELAY;
			}
		}
	}
}

float* getDelayedVals()
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
	float* delayedVal = (float*)malloc(sizeof(float)*2);

	// For channel A, k = 0 and for channel B, k = 1
	int k;
	for (k = 0; k < 2; k++) {
		int ind;	// Index of the delayed value in the buffer
		if ((int)channDelay > idx) {
			ind = MAX_DELAY - ((int)channDelay[k] - idx);
		} else {
			ind = idx - (int)channDelay;
		}
		delayedVal[k] = buffer[ind];
	}

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

  	CodecDataIn.UINT = ReadCodecData();

  	// Toggle the pitch shifter and upward/downward pitch change
  	Int32 SW = ReadSwitches();

  	if (SW != prevSW) {
		if (SW == 0) { 			// SW5 up, SW6 up:
			upwardPitch = 1;
			pitchShiftIsOn = 1;
			channDelay[A] = (float)MAX_DELAY;
			channDelay[B] = (float)MAX_DELAY/2;
			channGain[A] = 0.0;
			channGain[B] = 1.0;
		} else if (SW == 1) { 	// SW5 down, SW6 up:
			upwardPitch = 1;
			pitchShiftIsOn = 0;
		} else if (SW == 2) {	// SW5 up, SW6 down:
			upwardPitch = 0;
			pitchShiftIsOn = 1;
			channDelay[A] = 0.0;
			channDelay[B] = (float)MAX_DELAY/2;
  			channGain[A] = 0.0;
  			channGain[B] = 1.0;
		} else if (SW == 3) { 	// SW5 down, SW6 down:
			upwardPitch = 0;
			pitchShiftIsOn = 0;
		}
  	}

  	// Update delays and gains
  	updateDelays();
	updateGains();

  	// Update buffer
	buffer[idx++] = CodecDataIn.Channel[LEFT];
	if (idx > MAX_DELAY)
		idx = 0;

	// Prepare output
	if (pitchShiftIsOn) {
		float* delayedChann = getDelayedVals();
		CodecDataOut.Channel[LEFT] = channGain[A]*delayedChann[A] + channGain[B]*delayedChann[B];
	} else {
		CodecDataOut.Channel[LEFT] = CodecDataIn.Channel[LEFT];
	}
	CodecDataOut.Channel[RIGHT] = CodecDataIn.Channel[RIGHT];

	WriteCodecData(CodecDataOut.UINT);
}

