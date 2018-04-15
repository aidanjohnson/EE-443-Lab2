			.def _xcorr

_xcorr:			 	    	;auto (cross with self) correlation
			MV B4,A0		;N samples counter, n
			MV A6,B0		;K coefficient or shift counter, k
			MV A4,A3		;resets n+k to n
			MV A6,A5		;backup coefficient counter

corr:
			MV A4,A5		;x(n) pointer reset
			MV A4,A3		;x(n+k) pointer reset
			MV B4,A0		;resets samples counter
			SUB A0,B0,A0	;shifts N samples by k

shift:						;sets x(n+k) pointer
			ADD A3,4,A3		;shifts by next k
			SUB B0,1,B0		;decrements shift
	[B0]	B	shift
			NOP 5

			ZERO A1			;temp storage for each product
			ZERO B1			;temp storage for sum of products
			ZERO A2			;temp storage for multiplier
			ZERO B2			;temp storage for multiplicand

sum:						;correlation summation
			LDW *A5++,A2	;multiplier
			NOP 8
			LDW *A3++,B2	;multiplicand
			NOP 8
			MPYSP A2,B2,A1	;multiplies in1 and in2 at n and n+k sample index
			NOP 8
			ADDSP B1,A1,B1	;sums product
			NOP 8
			SUB A0,1,A0		;decrements samples counter
			NOP 8
	[A0]    B sum
			NOP 5

			STW B1,*B6++	;stores correlation coefficient and increments to next coefficient store
			NOP 8
			SUB A6,1,A6		;decrements coefficient counter
			MV A6,B0		;resets shift counter
			NOP 8
	[B0]    B	corr
			NOP 5

			B     B3			;returns
			NOP   5
