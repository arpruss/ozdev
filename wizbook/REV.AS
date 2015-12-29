global _rev,_ext_compare,_strlen,_line_buf,_ozputs,start_pos

_rev:
	ld	a,(_ext_compare)
	or	a
	jr	z,FixedExt
	push	ix
	ld	hl,_line_buf
	ld	de,_line_buf
	ld	b,0
he_top:
	ld	a,(hl)
	cp	'|'
	jr	z,is_ext
	ld	(de),a
	or	a
	jr	z,he_out
	inc	hl
	inc	de
	jp	he_top
is_ext:
	inc	hl
	ld	c,(hl)
	ld	ix,_chartable-34
	add	ix,bc
	ld	a,(ix)
	ld	(de),a
	inc	hl
	inc	de
	jp	he_top

he_out:
	pop	ix

FixedExt:
;; now measure and trim ;;
	xor	a
	ld	(start_pos),a
	ld	(_dorev),a

	ld	hl,-1
	push	hl

	call	_ozputsf

	pop	bc

	ld	a,239
	or	a
	sbc	a,l

	ld	(start_pos),a

	ld	a,1
	ld	(_dorev),a

	ld	hl,0
eos equ $-2
	ld	(hl),0	;; trim
;; now need to reverse the string ;;
        ld      bc,_line_buf
        or      a
        ld      e,l    ; de should point to end
        ld      d,h
        dec     de
        sbc     hl,bc  ; hl=length
        ld      b,l    ; b=length
        srl     b      ; b=length/2
        ret     z      ; no need to reverse

	ld	hl,_line_buf

revtop:
	ld	a,(hl)
	ex	af,af'
	ld	a,(de)
	ex	af,af'
	ld	(de),a
	ex	af,af'
	ld	(hl),a
	inc	hl
	dec	de
	djnz	revtop

	ret
