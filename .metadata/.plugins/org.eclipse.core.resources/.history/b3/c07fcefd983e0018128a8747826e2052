			.def _stack
            .align 256          ;align delay buffer 256-byte boundary
delays      .space 256          ;init 256-byte buffer with 0's
last_addr   .int last_addr
            .text		        ;code section

_stack:
            ;MV    A4,B6			;move input array
  			MV    B4,B1	        ;push loop count
  			MV	  B4,A1			;pop loop count
  			MV	  A4,B15
  			MV    B6,A15

push:					        ;push input into stack
            STB	  A4,*B15++[4]  	;store to stack
            SUB   B1,1,B1	    ;decrement count
    [B1]    B     push    	    ;branch to loop if count != 0
            NOP   5

pop:
			STB   B15,*A15++[4]
			;LDB   *--B15[4],A15
			SUB   B15,4,B15
			SUB   A1,1,A1
	[A1]    B     pop
			NOP   5
			B     B3
			NOP   5
