; undefined data instruction ".ent"
.ent LIST

; invalid label begining "1W"
.extern  1W

; missing a target operand for "add"
MAIN:		add	r3,

; invalid number for immediate addressing "#a"
LOOP:		prn	#a
		macro m1
		inc	r6
		mov	r3, W
		endm
		
; invalid ragister number "r16"
		lea	STR, r16
		
		m1
		sub	r1, r4
		bne	END
		cmp	vall, #-6
		
; invalid register number "blah" for index addressing
		bne	END[blah]

		dec	K
.entry MAIN

; undefined symbol "LOO"
		sub	LOO[r10],r14

; undefined operation name "sto"
END:		sto

; missing closing qout marks
STR:		.string "abcd

; saving a saved word error
add:		.string "abcd

; missing number after last ","
LIST:		.data	6,-9,

; no numbers after ".data"
		.data	

.entry K
K:		.data	31
.extern	vall
