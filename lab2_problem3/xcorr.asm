			.def _xcorr

_xcorr:			 	    	;cross correlation
			ZERO A1			;temp storage for each product
			ZERO B1			;temp storage for sum of products
			MV B4,A0		;N samples counter, n
			MV A6,B0		;K coefficient counter, k

corr:
			SUB A0,B0,A0	;shifts N samples by k
			LDW *A4,B4		;resets n+k to n
			ADD B4,B0,B4	;shifts by next k

sum:						;correlation summation
			LDW *A4,A2
			LDW *B4,B2
			MPY A2,B2,A1	;multiplies in1 and in2 at n and n+k sample index
			ADD B1,A1,B1	;sums product
			SUB A0,1,A0		;decrements samples counter

	[A0]    B sum
			STW B1,*B6
			ADD B6,4,B6
			SUB B0,1,B0		;decrements coefficient counter
			MV B4,A0		;resets samples counter

	[B0]    B	corr
