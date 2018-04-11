			.def _flip_512
            .align 256          ;align delay buffer 256-byte boundary
delays      .space 256          ;init 256-byte buffer with 0's
last_addr   .int   last_addr-2  ;point to bottom of delays buffer
            .text		        ;code section

_flip_512:			 	    	;flip
            MV	  B4,A1			;setup loop count
            MV    A4,B6			;move input array
            MVKL  0x00070040,B6 ;select A7 as pointer and (40=01000000 see Appendix B, Figure B.1)
            MVKH  0x00070040,B6 ;BK0 for 256=2^(7+1) bytes (128 shorts)

            MVC   B6,AMR        ;set address mode register AMR
	        MVKL  last_addr,A9  ;A9 last sample addr(lower 16 bits)
	        MVKH  last_addr,A9  ;last sample addr (higher 16 bits)
            LDW   *A9,A7        ;A7=last sample addr
            NOP   4             ;make sure no parallel execution
            STH   A4,*A7++	    ;newest sample-->last address

loop:					        ;begin loop
            LDH   *A7++,A4      ;A4
            SUB   A1,1,A1	    ;decrement count
    [A1]    B     loop    	    ;branch to loop if count # 0
            NOP   2