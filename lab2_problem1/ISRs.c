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

/* add any global variables here */
float xLeft, xRight, yLeft, yRight;
Uint32 oldest = 0; // index for buffer value
#define DELAY 0.5 // seconds
float BUFFER_LENGTH = DELAY * GetSampleFreq(); // buffer length in samples = sampling rate in Hz * delay in s
#pragma DATA_SECTION (buffer, "CE0"); // put "buffer" in SDRAM
volatile float buffer[DELAY][BUFFER_LENGTH]; // space for left + right
int DelaySamples = (int) BUFFER_LENGTH;  // can be manipulated by GEL file
volatile float gain = 1; /* set gain value for echoed sample */

void ZeroBuffer()
///////////////////////////////////////////////////////////////////////
// Purpose:   Initial fill of all buffer locations with 0.0
//
// Input:     None
//
// Returns:   Nothing
//
// Calls:     Nothing
//
// Notes:     None
///////////////////////////////////////////////////////////////////////
{
    int i;

    for(i=0; i < BUFFER_LENGTH; i++)  {
        buffer[LEFT][i] = 0.0;
        buffer[RIGHT][i] = 0.0;
        }
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
	/* add any local variables here */
	Uint32 newest;  // only used for infinite echo

 	if(CheckForOverrun())					// overrun error occurred (i.e. halted DSP)
		return;								// so serial port is reset to recover

  	CodecDataIn.UINT = ReadCodecData();		// get input data samples
	
	/* add your code starting here */
	Int32 SW = ReadSwitches();

	if(SW % 2 == 1) // ignore SW5 (= 2^0)
		SW -= 1;

	// check switches: SW6 = 2^1, SW7 = 2^2; 0 = up, 1 = down
	// LEDs: D4 = 2^0, D5 = 2^1; 0 = off, 1 = on
	if(SW == 2) { // SW6 down, SW7 up: right out off, D4 off and D5 on
		DelaySamples = 600;
		WriteLEDs(2);
	}
	else if(SW == 4) { // SW6 up, SW7 down: left out off, D4 on and D5 off
		DelaySamples = 400;
		WriteLEDs(1);
	}
	else if(SW == 6) { // SW6 and SW7 down: right and left out off, D4 and D5 off
		DelaySamples = 800;
		WriteLEDs(0);
	}
	else if(SW == 0) { // SW6 and SW7 up: right and left out on, D4 and D5 on
		DelaySamples = 200;
		WriteLEDs(3);
	}


	/****************************
	ECHO ROUTINE BEGINS HERE
	****************************/
	xLeft = CodecDataIn.Channel[LEFT];   // current LEFT input value to float
	xRight = CodecDataIn.Channel[RIGHT];   // current RIGHT input value to float

	buffer[LEFT][oldest] = xLeft;
	buffer[RIGHT][oldest] = xRight;
	newest = oldest;
	if (++oldest >= DelaySamples) // implement circular buffer
		oldest = 0;

	// use either FIR or IIR lines below

	// for FIR comb filter effect, uncomment next two lines
	yLeft = xLeft + (gain * buffer[LEFT][oldest]);
	yRight = xRight + (gain * buffer[RIGHT][oldest]);

	CodecDataOut.Channel[LEFT] = yLeft;   // setup the LEFT value
	CodecDataOut.Channel[RIGHT] = yRight; // setup the RIGHT value
	/*****************************/
	/* end your code here */

	WriteCodecData(CodecDataOut.UINT);		// send output data to  port
}