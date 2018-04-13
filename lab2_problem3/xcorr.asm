			.def _xcorr
            .align 256          ;align delay buffer 256-byte boundary
            .text		        ;code section

_xcorr:							;A4 = in, x(n); B6 = out, R(k)
			MV    A4,A2			;copies input to n pointer
  			MV    B4,B0	        ;B4 = samples, N
  			MV	  B4,A0			;A6 = autocorrelation coefficients, k
			ZERO  A1			;zeros k counter
			ZERO  B1			;zeros n counter
			ZERO  A9			;zeros accummulator
			ADD   A1,A0,A1		;initialises k counter
			ADD   B1,B0,B1		;initialises n counter
			MV    A2,A5			;n+k pointer

corr:							;finds k coefficients
            ADD   A5,A1,A5      ;shifts n+k pointer by k
            ADD   A1,4,A1		;increment k count
sum:					        ;sums N-k autocorrelated products
            MPY   A2,A5,A7  	;stores product
            SUM   A9,A7,A9		;adds to accummulator
            ADD   A2,4,A2		;increments n pointer
            SUB   B1,4,B1	    ;decrement n count
    [B1]    B     sum    	    ;branch to sum if count != 0
            NOP   5
            MV    A2,A4			;n pointer returns back to start
            SUB   A0,A1,B2      ;coefficients remaining
            STB   A9,*B6++[4]	;moves coefficient to output
	[A2]	B	  corr			;branch to corr if coefficients remaining != 0
			NOP   5

			B     B3			;returns
			NOP   5

