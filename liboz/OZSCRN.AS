; void ozscrolllinesclear(byte n);
psect lowpage
global _ozscrolllinesclear,__ozactivepage
_ozscrolllinesclear:
        pop     hl
        pop     de
        push    de
        push    hl

	in	a,(3)
	ld	c,a
	in	a,(4)
	ld	b,a
	push	bc

        ld      bc,(__ozactivepage)
	ld	a,c
	out	(3),a
	ld	a,b
	out	(4),a

;; multiply e by 30, and put in hl

        ld      l,e
        ld      h,0

        add     hl,hl ; hl=e*2
        ld      e,l
        ld      d,h   ; de=e*2
        add     hl,hl ; hl=e*4
        add     hl,hl ; hl=e*8
        add     hl,hl ; hl=e*16
        add     hl,hl ; hl=e*32
        sbc     hl,de ; hl=e*30

        ld      (num_to_clear),hl

        ld      e,l
        ld      d,h

        ld      hl,2400  ;; carry should be clear
        sbc     hl,de

        ld      c,l
        ld      b,h   ;; bc=2400-num_to_clear

        ld      hl,0a000h
        add     hl,de
	ld	de,0a000h

	ldir
	ld	de,300
num_to_clear equ $-2

        ld      c,e
        ld      b,d

        ld      hl,0a000h+2400
        or      a
        sbc     hl,de

	xor	a
	ld	e,a
rpt:
	ld	(hl),e
	inc	hl
	dec	bc
	ld	a,b
	or	c
	jr	nz,rpt

	pop	bc

	ld	a,c
	out	(3),a
	ld	a,b
	out	(4),a
	ret


