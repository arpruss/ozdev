;	floating relational operation - returns flags as though
;	a floating subtract was done.

	psect	text
	global	frelop

frelop:
	exx			;select	alternate reg set
	pop	hl		;return	address
	exx			;get other set again
	pop	bc		;low word of 2nd arg
	ex	de,hl		;put hi	word of	1st in de
	ex	(sp),hl		;get hi	word of	2nd in hl
	ex	de,hl		;hi word of 1st	back in	hl
	ld	a,h		;test for differing signs
	xor	d
	jp	p,2f		;the same, so ok

	ld	a,h		;get the sign of the LHS
	or	1		;ensure zero flag is reset, set sign flag
	pop	bc		;unjunk stack
	jp	1f		;return	with sign of LHS
2:
	ld	a,h		;test for differing exponents
	sub	d		;compare with the other
	jr	z,2f		;the same, go for mantissas

	xor	h		;complement sign flag if operands -ve
	or	1		;reset zero flag
	pop	bc		;unjunk stack
	jp	1f		;and return
2:
        bit     7,d             ; check for negative sign on one of them
        jr      z,DontFlip
        ex      de,hl
DontFlip:
        sbc     hl,de           ;set the flags

	pop	hl		;low word of 1st into hl again

	jr	nz,1f		;go return if not zero

        bit     7,d             ; check for negative sign on one of them again
        jr      z,DontFlip2
        ld      e,c
        ld      d,b
        ex      de,hl
        ld      c,e
        ld      b,d
DontFlip2:
        sbc     hl,bc           ;now set flags on basis of low word
	jr	z,1f		;if zero, all ok
	ld	a,2		;make non-zero
	rra			;rotate	carry into sign
	or	a		;set minus flag
	rlca			;put carry flag	back

1:
	exx			;get return address
	jp	(hl)		;and return with stack clean

