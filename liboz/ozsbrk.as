	psect	text
        global  _sbrk,__Hbss, _brk, HeapTop

;	NB This brk() does not check that the argument is reasonable.

_brk:
	pop	hl	;return address
	pop	de	;argument
	ld	(memtop),de	;store it
	push	de		;adjust stack
	jp	(hl)	;return

_sbrk:
	pop	bc
	pop	de
	push	de
	push	bc
	ld	hl,(memtop)
	ld	a,l
	or	h
	jr	nz,1f
	ld	hl,__Hbss
	ld	(memtop),hl
1:
        add     hl,de
	jr	c,2f		;if overflow, no room
        ld      bc,HeapTop+1    ; top of program memory
        sbc     hl,bc
	jr	c,1f
2:
        ld      hl,-1           ;no room
	ret

1:	ld	hl,(memtop)
	push	hl
	add	hl,de
	ld	(memtop),hl
	pop	hl
	ret

        psect   data
memtop: defw    0

