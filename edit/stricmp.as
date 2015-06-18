	psect	text
        global  _stricmp

;; based on Hi-Tech C library ;;

_stricmp:        pop     bc
	pop	de
	pop	hl
	push	hl
	push	de
	push	bc

1:      ld      a,(de)
        cp      'z'+1
        jr      nc,NotLC2
        cp      'a'
        jr      c,NotLC2
        and     0ffh-20h
NotLC2:
        ex      af,af'
        ld      a,(hl)
        cp      'z'+1
        jr      nc,NotLC
        cp      'a'
        jr      c,NotLC
        and     0ffh-20h
NotLC:
        ld      c,a
        ex      af,af'
        cp      c
	jr	nz,2f
	inc	de
	inc	hl
	or	a
	jr	nz,1b
	ld	hl,0
	ret

2:	ld	hl,1
	ret	nc
	dec	hl
	dec	hl
	ret

