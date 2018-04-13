			.def _stack
            .align 256          ;align delay buffer 256-byte boundary
            .text		        ;code section

_stack:
  			MV    B4,B1	        ;push loop count
  			MV	  B4,A1			;pop loop count
  			MV	  A4,B15		;stack pointer -> front of input array

push:					        ;push input into stack with stack pointer
            LDW   *B15++,A6	;assumes 4 Byte float i/o given by A6
            SUB   B1,1,B1	    ;decrement count
    [B1]    B     push    	    ;branch to loop if count != 0
            NOP   5

pop:							;pops stack to output array
			LDW   *--B15,A0		;popped by stack pointer
			NOP	  5
			STW   A0,*B6++		;popped to output array pointer
			SUB   A1,1,A1		;decrement count
	[A1]    B     pop
			NOP   5
			B     B3			;returns
			NOP   5
