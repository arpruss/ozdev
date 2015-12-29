*Include macros.inc

newline_width equ 6
newline_char equ 13
psect	beforebss

*include rev.as

	global	ScrCharSet,_ozputsf ;; no system calls
        global  __ozmodel,_ozdetectmodel,_line_buf,_chartable
        global _safety,start_pos,_dorev
_line_buf: defs 251
        defb 0
        defb 0
_safety: defb 233
_chartable: defs 94

global LowerFontPage2,LowerFontPage1,HigherFontPage1,HigherFontPage2
global __ozfontpointers

__ozfontpointers:
	defw	Font0
	defw	Font1
	defw	Font2
	defw	Font3
	defw	Font1
	defw	Font1

Font0:	;; PC_LARGE
	defb	0eh
LowerFontPage2	equ $-1
	defb	0
	defw	0ab10h
	defw	0ab10h+310h
	defb	13
	defb	0ffh

Font1:	;; PC_PLANE:
	defb	0eh
LowerFontPage1	equ $-1
	defb	0
	defw	0a000h
	defw	0a000h+310h
	defb	8
	defb	0ffh

Font2:	 ;; EO7LARGE
	defb	0fh
HigherFontPage2 equ $-1
	defb	0
	defw	0ab10h
	defw	0ab10h+310h
	defb	13
	defb	0ffh

Font3:	 ;; EO7PLANE:
	defb	0fh
HigherFontPage1 equ $-1
	defb	0
	defw	0a000h
	defw	0a000h+310h
	defb	8
	defb	0ffh

DoInit:
	ld	a,1
	ld	(init),a
	ld	a,(__ozmodel)
	xor	0ffh
	call	z,_ozdetectmodel
	ld	a,(__ozmodel)
	and	4
	jr	z,DidInit  ;; not multilingual, so all is OK
	ld	a,1eh
	ld	(LowerFontPage1),a
	ld	(LowerFontPage2),a
	inc	a
	ld	(HigherFontPage1),a
	ld	(HigherFontPage2),a
	jr	DidInit

; int ozputs(int x, int y, char *string);
_ozputsf:
	ld	a,(init)
	or	a
	jr	z,DoInit
DidInit:
        ld      a,(_dorev)
        or      a
        call    nz,_rev

	push	iy
	ld	iy,2
	add	iy,sp
	push	ix

	ld	a,(ScrCharSet)
	cp	6
	jr	c,FontOK
	ld	a,1
FontOK:
	add	a,a
	ld	d,0
	ld	e,a
	ld	hl,__ozfontpointers
	add	hl,de
	ld	e,(hl)
	inc	hl
	ld	d,(hl)

	push	de
	pop	ix

;; page in font
	ld	a,(ix)
	out	(3),a
	ld	a,(ix+1)
	out	(4),a

	ld	l,(ix+2)
	ld	h,(ix+3)
	ld	(font_len_offset),hl
	ld	l,(ix+4)
	ld	h,(ix+5)
	ld	(font_offset),hl
	ld	(font_offset_double),hl
        ld      e,(ix+6) ;; height
	ld	a,(ix+7) ;; charset mask
	ld	(charset_mask),a

	ld	a,e
	ld	(height),a
	ld	(height_double),a

	ld	hl,400h-4

	ld	c,1
	out	(c),l
	inc	c
	out	(c),h  ;; page in screen
                     
	ld	l,(iy+2)

        ld      a,l
        cp      0ffh
        jp      z,MeasureOnly

	ld	h,0
	add	hl,hl
	ld	c,l
	ld	b,h	 ; bc=2*y
	add	hl,hl	 ; hl=4*y
	add	hl,hl	 ; hl=8*y
	add	hl,hl	 ; hl=16*y
	add	hl,hl	 ; hl=32*y
	sbc	hl,bc	 ; hl=30*y

	ld	bc,08000h
	add	hl,bc

	push	hl
	pop	ix	 ; ix=screen-offset for y

        ld      a,0
_ozputsf_clear equ $-1
global _ozputsf_clear
        or      a
        jr      z,DontClear

        ex      de,hl
        ld      a,(height)
        ld      h,c  ; c=0 here
        ld      l,a

        add     hl,hl  ; hl=2*height
        ld      c,l
        ld      b,h
        add     hl,hl  ; hl=4*height
        add     hl,hl  ; hl=8*height
        add     hl,hl  ; hl=16*height
        add     hl,hl  ; hl=32*height
        sbc     hl,bc

        ld      c,l
        ld      b,h
        dec     bc

        ex      de,hl
        xor     a
        ld      (hl),a
        ld      e,l
        ld      d,h
        inc     de

        ldir

DontClear:

        ld      bc,0     ; x position
start_pos equ $-2
global _ozputsf_start_pos
_ozputsf_start_pos equ start_pos
        ld      (EndXPos),bc
        ld      de,_line_buf

DoPrintLoop:
	ld	a,(de)	     ;; get character from string
	and	0ffh
charset_mask equ $-1
	jp	z,done

	push	de	     ;; string pointer
	push	bc	     ;; x-pos

	cp	'|'
_ext_compare equ $-1
global _ext_compare
	jp	nz,SkipCompare

	pop	bc
	pop	de
	inc	de
	push	de
	push	bc

	ld	a,(de)
	ld	c,a
	ld	b,0
        ld      hl,_chartable-34
	add	hl,bc
        ld      l,(hl)
        ld      a,(charset_mask)
        and     l

SkipCompare:

	ld	hl,0000h     ;; self modifying code
font_len_offset equ $-2
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
	add	hl,bc

;; now hl=width, iy=character record position
DoPrintChar:

	push	hl
	pop	iy	     ;; iy=font character record position

	ld	a,(hl)	; width
	ld	l,a
	ld	h,b	; h=0 as b=0 still


	pop	bc	; x-position
	push	bc

	ld	e,c
	add	hl,bc

        ex      af,af'
        ld      a,l
        cp      240
        jp      nc,DonePop2
        ld      (EndXPos),hl
        ex      af,af'

	cp	9	; is width more than 8
	jp	nc,DoubleWidth

	ld	a,e	; low byte of x-position
	and	7
	ld	e,a
	add	a,a
	add	a,e

	ld	e,a
	ld	d,0

	ld	hl,NoRotation
	sbc	hl,de
	ld	(RotateBarrelLoc),hl

	ld	l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset equ $-2	  ;;
	add	hl,bc	  ;; hl=position of font data for character

	push	hl
	pop	iy	  ;; iy=font data pointer

	pop	bc	  ;; x-position of start

	srl	c
	srl	c
	srl	c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

	ld	c,00  ;; character row counter (self-modifying)
height equ $-1	      ;;

InnerCharLoop:
	ld	e,0
	ld	a,(iy)

	jp	0000
RotateBarrelLoc equ $-2

	rla
	rl	e

	rla
	rl	e

	rla
	rl	e

	rla
	rl	e

	rla
	rl	e

	rla
	rl	e

	rla
	rl	e

NoRotation:
	or	(hl) ;; get from screen
	ld	(hl),a
	inc	hl
	ld	a,(hl)
	or	e
	ld	(hl),a
	ld	de,29
	add	hl,de
	inc	iy
	dec	c
	jp	nz,InnerCharLoop

EndOfCharPut:
	ld	bc,0000     ;; self-modifying code
EndXPos equ $-2 	    ;;

	pop	de
	inc	de

	jp	DoPrintLoop

done:
FinalClearTop:
cleanup_page:
	pop	ix
	pop	iy

	ld	a,7
	out	(3),a
	ld	a,6-4
	out	(1),a
	ld	a,4
	out	(2),a
	out	(4),a

        ld      hl,(EndXPos)

	ret

MeasureOnly:
        ld      de,_line_buf
        ld      hl,(start_pos)
        ld      b,0
MOtop:
        ld      a,(de)
        or      a
        jr      z,MODone
        ld      c,a
        ld      ix,(font_len_offset)
        add     ix,bc
        add     ix,bc
        add     ix,bc
        ld      c,(ix)
        add     hl,bc
        ld      a,l
        cp      240
        jr      nc,OverFlow
        inc     de
        jp      MOtop

OverFlow:
        sbc     hl,bc
MODone:
        ld      (EndXPos),hl
        ld      (eos),de
        jp      done

DonePop2:
	pop	hl  ; x position
	pop	de
	jr	cleanup_page

DoubleWidth:
	ld	a,e	; low byte of x-position
	and	7
	ld	(low_nibble_double),a

	ld	l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset_double equ $-2	 ;;
	add	hl,bc	  ;; hl=position of font data for character
	push	hl
	pop	iy	  ;; iy=font data pointer

	pop	bc	  ;; x-position of start

	srl	c
	srl	c
	srl	c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

	push	ix

	undoc_ix
	ld	l,00  ;; ld ixl,00 : character row counter (self-modifying)
height_double equ $-1	     ;;

InnerCharLoop_double:
	ld	b,00  ;; self-modifying code
low_nibble_double equ $-1    ;;

	inc	iy
	ld	e,(iy)
	ld	c,0

	ld	a,b
	or	a
	ld	a,(iy-1)
	jr	z,NoCharacterShift_double
ShiftByte_double:
	rla
	rl	e
	rl	c
	djnz	ShiftByte_double
NoCharacterShift_double:
	or	(hl) ;; get from screen
	ld	(hl),a
	inc	hl
	ld	a,(hl)
	or	e
	ld	(hl),a
	inc	hl
	ld	a,(hl)
	or	c
	ld	(hl),a
	ld	de,28
	add	hl,de
	inc	iy
	undoc_ix
	dec	l   ;; dec ixl
	jp	nz,InnerCharLoop_double
	pop	ix
	jp	EndOfCharPut

;Masks:  defb 1,3,7,15,31,63,127,255
init:	defb 0

;_line_buf:
;	 defm 'H'
;	 defb 0
;_check: defw 0
;global _check

global set_a000_400,_scrollbar_pos,_bar,_unbar,restore_a000
	psect lowpage

doset:
	ld    hl,(_scrollbar_pos)
	call  set_a000_400
	ld    b,3
	ld    de,30
	ret

_bar:
	call  doset
bar_top:
	ld    a,(hl)
	or    80h
	ld    (hl),a
	add   hl,de
	djnz  bar_top
	jp    restore_a000

_unbar:
	call  doset
bar_top2:
	ld    a,(hl)
	and   7fh
	ld    (hl),a
	add   hl,de
	djnz  bar_top2
	jp    restore_a000

