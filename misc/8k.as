;	Run time start off for Small C.

        global _main,$gint

        psect text

        DEFB 00h, 64h, 0Fh, 0FEh, 31h, 26h, 48h, 41h
        DEFB 30h, 32h, 30h, 3Ah, 0FEh, 2Ah, 3Ah, 0FEh
        DEFB 60h, 0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh
        DEFB 00h, 00h, 00h, 00h, 00h, 00, 00, 00

start:
        call    _main    ;call main program
        ret

;
;*****************************************************
;						     *
;	runtime	library	for small C compiler	     *
;						     *
;	c.s - runtime routine for basic C code	     *
;						     *
;		Ron Cain			     *
;						     *
;*****************************************************

; fetch int from (HL)
$gint:
        ld      a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ret

