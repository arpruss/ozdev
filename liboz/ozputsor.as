*Include macros.inc

        psect   lowpage
        global  ScrCharSet,_ozputsor,_ozputchor ;; no system calls
        global  _ozputsgetend,__ozmodel,_ozdetectmodel
        global  __ozactivepage
        global  $end$pointer

;; strlen  equ     7f0ch
;; DisplayStringAt  equ 7f4ch
;; Portable if ScrCharSet is portable ;;

DoInit:
        ld      a,1
        ld      (init),a
        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel
        ld      a,(__ozmodel)
        and     4
        jr      z,DidInit  ;; not multilingual, so all is OK
        ld      a,1eh
        ld      (LowerFontPage1),a
        ld      (LowerFontPage2),a
        inc     a
        ld      (HigherFontPage1),a
        ld      (HigherFontPage2),a
        jr      DidInit
SetLarge:
PC_PLANE:
	ld	a,0eh
LowerFontPage1  equ $-1
	ld	(font_page),a
	ld	hl,0a000h
        ld      e,8
        jr      set_ozlen_page
PC_LARGE:
	ld	a,0eh
LowerFontPage2  equ $-1
        ld      (font_page),a
        ld      (font_page_double),a
	ld	hl,0ab10h
        ld      e,13
        jr      set_ozlen_page
EO7PLANE:
	ld	a,0fh
HigherFontPage1 equ $-1
        ld      (font_page),a
	ld	hl,0a000h
        ld      e,8
        jr      set_ozlen_page

; int ozputs(int x, int y, char *string);
_ozputsor:
        ld      a,(init)
        or      a
        jr      z,DoInit
DidInit:
        push    iy
	ld	iy,2
	add	iy,sp
	push	ix
	in	a,(3)
	ld	(restore_page_low),a
	in	a,(4)
	ld	(restore_page_high),a

        ld      hl,__ozactivepage
        ld      a,(hl)
        ld      (activepage_low),a
        ld      (activepage_double_low),a
        inc     hl
        ld      a,(hl)
        ld      (activepage_high),a
        ld      (activepage_double_high),a

	ld	a,(ScrCharSet)
	cp	1
	jr	z,PC_PLANE
	or	a
	jr	z,PC_LARGE
	cp	3
	jr	z,EO7PLANE
;EO7LARGE
        ld      a,0fh
HigherFontPage2 equ $-1
        ld      (font_page),a
        ld      (font_page_double),a
	ld	hl,0ab10h
        ld      e,13  ;; height
set_ozlen_page:
	ld	(font_len_offset),hl
        ld      a,(iy+5)
        or      a
        jp      nz,LengthOnly

        ld      bc,310h
        add     hl,bc
        ld      (font_offset),hl
        ld      (font_offset_double),hl

        ld      a,80
        ld      c,(iy+4)
        sub     c            ;; number of rows available
        jp      m,LengthOnly
        jp      z,LengthOnly
        cp      e
        jp      nc,HeightOK
        ld      e,a
HeightOK:
        ld      a,e
        ld      (height),a
        ld      (height_double),a

	ld	e,(iy+6)
	ld	d,(iy+7)   ; address of string

	ld	l,(iy+4)
        ld      h,0
	add	hl,hl
	ld	c,l
	ld	b,h	 ; bc=2*y
	add	hl,hl	 ; hl=4*y
	add	hl,hl	 ; hl=8*y
	add	hl,hl	 ; hl=16*y
	add	hl,hl	 ; hl=32*y
	sbc	hl,bc	 ; hl=30*y
        ld      bc,0a000h
        add     hl,bc
	push	hl
	pop	ix	 ; ix=screen-offset for y
        ld      c,(iy+2)   ; x-position
        ld      b,(iy+3)
DoPrintLoop:
        ld      a,(de)       ;; get character from string
	or	a
        jp      z,done
        push    de           ;; string $end$pointer
        push    bc           ;; x-pos
	ld	hl,0000h     ;; self modifying code
font_len_offset equ $-2
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
        add     hl,bc
	push	hl
        pop     iy           ;; iy=font character record position
	ld	a,(font_page)
	out	(3),a
	xor	a
        out     (4),a
	ld	a,(hl)	; width
        ld      l,a
        ld      h,b     ; h=0 as b=0 still
        pop     bc      ; x-position
	push	bc
        ld      e,c
	add	hl,bc
	ld	(EndXPos),hl
        ld      bc,239
	sbc	hl,bc
        jp      nc,DonePop2
        cp      9       ; is width more than 8
        jp      nc,DoubleWidth
        ld      a,e
        and     7
        ld      (low_nibble),a
	ld	l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset equ $-2	  ;;
        add     hl,bc     ;; hl=position of font data for character
	push	hl
        pop     iy        ;; iy=font data $end$pointer

	pop	bc	  ;; x-position of start

        srl     c
        srl     c
        srl     c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

	ld	c,00  ;; character row counter (self-modifying)
height equ $-1	      ;;

InnerCharLoop:
        ld      b,00  ;; self-modifying code
low_nibble equ $-1    ;;

	ld	a,00  ;; self-modifying code
font_page equ $-1     ;;
	out	(3),a
	xor	a
	out	(4),a

        ld      e,a
        ld      d,(iy)

        ld      a,00
activepage_low  equ $-1
	out	(3),a ; a=0
        ld      a,00
activepage_high equ $-1
	out	(4),a ; page 400h

	ld	a,b
	or	a
        jp      z,NoCharacterShift
ShiftByte:
        rl      d
        rl      e
	djnz	ShiftByte
NoCharacterShift:
	ld	a,(hl) ;; get from screen
	or	d
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
	ld	a,00	    ;; self-modifying code
restore_page_low equ $-1    ;;
	out	(3),a
	ld	a,00	    ;; self-modifying code
restore_page_high equ $-1   ;;
	out	(4),a

	jp	DoPrintLoop

done:
        ld      ($end$pointer),de
        ld      l,c
        ld      h,b

cleanup_page:
	pop	ix
	pop	iy
	ld	a,(restore_page_low)
	out	(3),a
	ld	a,(restore_page_high)
	out	(4),a
	ret

DonePop2:
        pop     hl  ; x position
        pop     de
        ld      ($end$pointer),de
        jr      cleanup_page

DoubleWidth:
        ld      a,e     ; low byte of x-position
        and     7
        ld      (low_nibble_double),a
        ld      l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset_double equ $-2       ;;
        add     hl,bc     ;; hl=position of font data for character
	push	hl
        pop     iy        ;; iy=font data $end$pointer

	pop	bc	  ;; x-position of start

        srl     c
        srl     c
        srl     c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

        push    ix

        undoc_ix
        ld      l,00  ;; ld ixl,00 : character row counter (self-modifying)
height_double equ $-1        ;;

InnerCharLoop_double:
        ld      b,00  ;; self-modifying code
low_nibble_double equ $-1    ;;

	ld	a,00  ;; self-modifying code
font_page_double equ $-1     ;;
	out	(3),a
	xor	a
	out	(4),a

        ld      d,(iy)
        inc     iy
        ld      e,(iy)
        ld      c,0

        ld      a,00
activepage_double_low equ $-1
        out     (3),a ;
        ld      a,00
activepage_double_high equ $-1
	out	(4),a ; page 400h

	ld	a,b
	or	a
        jp      z,NoCharacterShift_double
ShiftByte_double:
        rl      d
        rl      e
        rl      c
        djnz    ShiftByte_double
NoCharacterShift_double:
	ld	a,(hl) ;; get from screen
	or	d
	ld	(hl),a
	inc	hl
	ld	a,(hl)
	or	e
	ld	(hl),a
        inc     hl
	ld	a,(hl)
        or      c
        ld      (hl),a
        ld      de,28
	add	hl,de
	inc	iy
        undoc_ix
        dec     l
        jp      nz,InnerCharLoop_double
        pop     ix
        jp      EndOfCharPut

LengthOnly:
	ld	e,(iy+6)
	ld	d,(iy+7)   ; address of string

        ld      c,(iy+2)   ; x-position
        ld      b,(iy+3)
DoCountLoop:
        ld      a,(de)       ;; get character from string
	or	a
        jp      z,doneCounting
        push    bc           ;; x-pos
        ld      hl,(font_len_offset)
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
        add     hl,bc
	ld	a,(font_page)
	out	(3),a
	xor	a
        out     (4),a
	ld	a,(hl)	; width
        ld      l,a
        ld      h,b     ; h=0 as b=0 still
        pop     bc      ; x-position
	add	hl,bc
        ld      c,l
        ld      b,h
	inc	de
        ld      a,(restore_page_low)
	out	(3),a
        ld      a,(restore_page_high)
	out	(4),a

        jp      DoCountLoop

doneCounting:
        ld      l,c
        ld      h,b
        jp      cleanup_page

ozputchbuf:
        defb 0
        defb 0

; int ozputs(int x, int y, char c);
_ozputchor:
        ld   hl,6
        add  hl,sp
        ld   a,(hl)
        ld   (ozputchbuf),a
        ld   bc,ozputchbuf
        ld   (hl),c
        inc  hl
        ld   (hl),b
        jp   _ozputsor

psect       bss
init:       defs 1
