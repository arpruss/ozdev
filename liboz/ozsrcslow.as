global $aux_pagein,$aux_pageout,__ozauxstart,_ozcrc16auxmem

psect beforebss

empty:
	push	bc	;; bc=0
	pop	hl
	ret

_ozcrc16auxmem:
;; unsigned ozcrc16auxmem(unsigned start,unsigned length);

	pop	de
        pop     hl ;; start
        pop     bc ;; length
	push	bc
	push	hl
	push	de

	ld	a,c
	or	b
	jr	z,empty

	ld	de,8000h
	add	hl,de
	ld	de,(__ozauxstart)
	add	hl,de

        call    $aux_pagein

        ld      de,0ffffh

;; hl=data, bc=length, de=crc

crcloop:
        push    bc
        ld      c,(hl)
        ld      b,8
innerloop:

        ld      a,e
        xor     c

        srl     d
        rr      e

        rra

        jp      nc,end_inner_loop

        ld      a,e
        xor     8
        ld      e,a
        ld      a,d
        xor     84h
        ld      d,a

end_inner_loop:

        srl     c
        djnz    innerloop

        pop     bc

        inc     hl
        dec     bc
        ld      a,c
        or      b
        jp      nz,crcloop

        ld      a,d
        cpl
        ld      l,a
        ld      a,e
        cpl
        ld      h,a
        jp      $aux_pageout
