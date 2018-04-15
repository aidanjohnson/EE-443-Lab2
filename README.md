# EE 443 Lab 2

## Problem 1
Status: Complete

Complete:
- Generates echo effect (using textbook code Ch. 10.5.1--see CD/code/chapter_10/ccs/Echo/ISRs_[A/B/C].c)

Incomplete:
- Confirmation of buffer delay 

## Problem 2
Status: Complete

Complete:
- Flips input data set
- Tested with [0:1:511] (or range(0,512,1)) input -> [511: -1 :0] (or range(511,-1,-1)) output

Incomplete:
- N/A

## Problem 3
Status: In Progress

Complete:
- Functioning autocorrelation assembly code that computes using the algorithm: For each correlation coefficient k, sum the product of the input function x(n) and it shifted by k x(n+k) for n between [0,N-k], where N is the number of samples of x(n).
- Multiplication seems to be correct when compared to [Scilab](https://www.scilab.org/) correlation:
--> x = [0:9]; y = xcorr(x, x, 5); y = y(1:5)
- Seems to work with MPYSP and ADDSP since working with floats

Incomplete:
- N/A

## Problem 4
Status: In Progress

Complete:
- N/A

Incomplete:
- Pitch Shifter (entire problem)


## Textbook
[2nd ed.](https://doc.lagout.org/programmation/Multi-Language/Real-Time%20Digital%20Signal%20Processing%20from%20MATLAB%20to%20C%20with%20the%20TMS320C6x%20DSPs%20%282nd%20ed.%29%20%5BWelch%2C%20Wright%20%26%20Morrow%202011-12-22%5D.pdf)
