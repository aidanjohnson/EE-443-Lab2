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
#define BIAS 650

int idx = 0;							// Index of the end of the buffer
float channDelay[2];					// Delay of each channel
float channGain[2];						// Gain of each channel
int channGainInc[2] = {1, 0};			// 1 if channel gain is increasing, 0 if decreasing
int upwardPitch = 1;					// 1 if upward pitch change, 0 if downward pitch change
int pitchShiftIsOn = 1;					// 1 if pitch shifter is enabled, 0 is disabled
int prevSW = -1;						// Previous switch values
float buffer[MAX_DELAY + 1];			// 0 to 700


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
	// Channel A
	if (upwardPitch) {
		if (channDelay[A] <= 0) {
			channDelay[A] = (float)MAX_DELAY;
		} else {
			channDelay[A] -= 0.5;
		}
	} else {
		if (channDelay[A] >= (float)MAX_DELAY) {
			channDelay[A] = 0.0;
		} else {
			channDelay[A] += 0.5;
		}
	}

	// Channel B
	if (channDelay[A] >= (float)MAX_DELAY/2) {
		channDelay[B] = channDelay[A] - (float)MAX_DELAY/2;
	} else {
		channDelay[B] = channDelay[A] + (float)MAX_DELAY/2;
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
	// Channel A
	if (channGainInc[A]) {
		if (channGain[A] >= 1) {
			channGainInc[A] = 0;
			channGain[A] -= (float)1/MAX_DELAY;
		} else {
			channGain[A] += (float)1/MAX_DELAY;
		}
	} else {
		if (channGain[A] <= 0) {
			channGainInc[A] = 1;
			channGain[A] += (float)1/MAX_DELAY;
		} else {
			channGain[A] -= (float)1/MAX_DELAY;
		}
	}

	// Channel B
	channGain[B] = 1 - channGain[A];
}

float getDelayedVals(int k)
///////////////////////////////////////////////////////////////////////
// Purpose:   Gets the delayed values of a channel
//
// Input:     The channel index
//
// Returns:   The delayed value of the channel
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
	if ((int) channDelay[k] > idx) {
		ind = MAX_DELAY - ((int) channDelay[k] - idx);
	} else {
		ind = idx - (int) channDelay[k];
	}
	delayedVal = buffer[ind];

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

		prevSW = SW;
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
		float channADelayed = getDelayedVals(A);
		float channBDelayed = getDelayedVals(B);
		CodecDataOut.Channel[LEFT] = channGain[A]*channADelayed + channGain[B]*channBDelayed - 2*BIAS;
	} else {
		CodecDataOut.Channel[LEFT] = CodecDataIn.Channel[LEFT];
	}
	CodecDataOut.Channel[RIGHT] = CodecDataIn.Channel[RIGHT];

	WriteCodecData(CodecDataOut.UINT);
}

