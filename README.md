# EE 443 Lab 2

## Problem 1
Status: In Progress

Complete:
- generates echo effect 

Incomplete:
- confirmation of buffer delay 

## Problem 2
Status: In Progress

Complete:
- N/A

Incomplete:
- C-callable assembly code for flipping
- ** A better explanation on circular buffering: http://www.ti.com/lit/an/spra645a/spra645a.pdf **
- My idea:
1) 512-value float array (4 bytes per entry) passed to .asm function
2) pointer on input array (starting at end and descending), moving address to each value to pointer on output array (starting at beginning and ascending)
3) return values as array, with an order corresponding to the output array

## Problem 3
Status: In Progress

Complete:
- N/A

Incomplete:
- C-callable assembly code for autocorrelation

## Problem 4
Status: In Progress

Complete:
- N/A

Incomplete:
- ?Pitch Shifter?


## Textbook
[2nd ed.] https://doc.lagout.org/programmation/Multi-Language/Real-Time%20Digital%20Signal%20Processing%20from%20MATLAB%20to%20C%20with%20the%20TMS320C6x%20DSPs%20%282nd%20ed.%29%20%5BWelch%2C%20Wright%20%26%20Morrow%202011-12-22%5D.pdf
