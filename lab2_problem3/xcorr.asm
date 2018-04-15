			.def _xcorr

_xcorr:			 	    	;auto (cross with self) correlation
			MV B4,A0		;N samples counter, n
			MV A6,B0		;K coefficient or shift counter, k
			MV A4,A3		;resets n+k to n

corr:

shift:
			ADD A3,4,A3		;shifts by next k
			SUB B0,1,B0		;decrements shift
	[B0]	B	shift

			NOP 5

			MV A6,B0		;resets shift counter
			SUB A0,B0,A0	;shifts N samples by k
			ZERO A1			;temp storage for each product
			ZERO B1			;temp storage for sum of products
			ZERO A2
			ZERO B2

sum:						;correlation summation
			LDW *A4++,A2		;multiplier
			LDW *A3++,B2		;multiplicand
			NOP 8
			MPY A2,B2,A1	;multiplies in1 and in2 at n and n+k sample index
			NOP 1
			ADD B1,A1,B1	;sums product
			SUB A0,1,A0		;decrements samples counter
	[A0]    B sum
			NOP 5

			STW B1,*B6		;stores correlation coefficient
			ADD B6,4,B6		;increments to next coefficient store
			SUB B0,1,B0		;decrements coefficient counter
			MV B4,A0		;resets samples counter
	[B0]    B	corr
