			.def _xcorr

_xcorr:			 	    	;cross correlation
			ZERO A1			;stores each product
			MV A6,A0		;N samples counter, n
			MV B6,B0		;K coefficient counter, k

corr:
			SUB A0,B0,A0	;shifts N samples by k
			LDW *A4,B4		;resets n+k to n
			ADD B4,B0,B4	;shifts by next k

sum:						;correlation summation
			MPY A4,B4,A1	;multiplies in1 and in2 at n and n+k sample index
			ADD A8,A1,A8	;sums product
			SUB A0,1,A0		;decrements samples counter

	[A0]    B sum

			SUB B0,1,B0		;decrements coefficient counter
			MV A6,A0		;resets samples counter

	[B0]    B	corr
