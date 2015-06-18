*Include macros.inc

        psect   lowpage

        global  ScrCharSet,_ozputs,_ozputch ;; no system calls
        global  _ozputsgetend,__ozmodel,_ozdetectmodel
        global  __ozactivepage,__ozfontpointers
        global  $end$pointer,restore_a000
        global LowerFontPage2,LowerFontPage1,HigherFontPage1,HigherFontPage2
        global __ozfontpointers

;; strlen  equ     7f0ch
;; DisplayStringAt  equ 7f4ch
;; Portable if ScrCharSet is portable ;;

_ozputsgetend:
        ld      hl,0  ;; self-mod
$end$pointer equ    $-2   ;;
        ret


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

; int ozputs(int x, int y, char *string);
_ozputs:
        ld      a,(init)
        or      a
        jr      z,DoInit
DidInit:
        push    iy
	ld	iy,2
	add	iy,sp
	push	ix

        exx
        ld      de,(__ozactivepage)

        ld      l,(iy+6)
        ld      h,(iy+7)   ; address of string
        exx


	ld	a,(ScrCharSet)
        and     7
        add     a,a
        ld      d,0
        ld      e,a
        ld      hl,__ozfontpointers
        add     hl,de
        ld      e,(hl)
        inc     hl
        ld      d,(hl)

        push    de
        pop     ix

        ld      a,(ix)
        ld      (font_page),a
        ld      a,(ix+1)
        ld      (font_page_hi),a

        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      (font_len_offset),hl
        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      (font_offset),hl
        ld      (font_offset_double),hl
        ld      e,(ix+6) ;; height
        ld      a,(ix+7) ;; charset mask
        ld      (charset_mask),a

        ld      a,(iy+5)
        or      a
        jp      nz,LengthOnly

        ld      a,80
        ld      c,(iy+4)
        sub     c            ;; number of rows available

        jp      c,LengthOnly
        jp      z,LengthOnly
        cp      e
        jp      nc,HeightOK
        ld      e,a
HeightOK:
        ld      a,e
        ld      (height),a
        ld      (height_double),a


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
        exx
DoPrintLoop:
        ld      a,(hl)       ;; get character from string
        or      a
        jp      z,done
        and     0ffh
charset_mask equ $-1
        exx
        push    bc           ;; x-pos
	ld	hl,0000h     ;; self modifying code
font_len_offset equ $-2
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
        add     hl,bc
        push    hl
        pop     iy           ;; iy=font character record position
        ld      a,(font_page)
	out	(3),a
        ld      a,(font_page_hi)
        out     (4),a
	ld	a,(hl)	; width
        ld      l,a
        ld      h,b     ; h=0 as b=0 still
        pop     bc      ; x-position
	push	bc
        ld      e,c
	add	hl,bc
        ld      (EndXPos),hl
        ld      bc,240
	sbc	hl,bc
        jp      nc,DonePop2
        cp      9       ; is width more than 8
        jp      nc,DoubleWidth
        ld      l,a     ; width
	ld	h,b	; h=0 as b=0 still  ;; hl=width
        ld      bc,Masks-1
	add	hl,bc
        ld      d,(hl)  ; mask for correct width
        ld      a,e     ; low byte of x-position
        ld      e,0
        and     7
        ld      (low_nibble),a
        jr      z,EndRotMask
        ld      b,a
RotRightMask:
        rl      d   ;; carry is clear because of "or a" and "dec a"
        rl      e
        djnz    RotRightMask
EndRotMask:


	ld	a,d
        cpl
	ld	(mask1),a
	ld	a,e
        cpl
        ld      (mask2),a


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

        ld      a,0  ;; self-modifying code
font_page equ $-1
	out	(3),a
        ld      a,0
font_page_hi equ $-1
	out	(4),a

        ld      e,0
        ld      d,(iy)


        exx
        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a
        exx

	ld	a,b
	or	a
        jr      z,NoCharacterShift
ShiftByte:
        rl      d
        rl      e
	djnz	ShiftByte
NoCharacterShift:
	ld	a,(hl) ;; get from screen
        and     00     ;; self-modifying code
mask1 equ $-1	       ;;
	or	d
	ld	(hl),a
	inc	hl
	ld	a,(hl)
        and     00     ;; self-modifying code
mask2 equ $-1	       ;;
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
        exx
        inc     hl
        ld      a,7         ;; restore original page
        out     (3),a
        ld      a,4
        out     (4),a
	jp	DoPrintLoop
done:
        ld      ($end$pointer),hl
        exx

        ld      l,c
        ld      h,b

cleanup_page:
	pop	ix
	pop	iy
        jp      restore_a000

DonePop2:
        pop     hl  ; x position
        exx
        ld      ($end$pointer),hl
        exx
        jr      cleanup_page

DoubleWidth:
        sub     8
        ld      l,a     ; width-8
	ld	h,b	; h=0 as b=0 still  ;; hl=width
        ld      bc,Masks-1
	add	hl,bc
        ld      a,e     ; low byte of x-position
        and     7
        ld      (low_nibble_double),a
        ld      d,0ffh
        ld      e,(hl)  ; mask for correct width
        ld      c,0
        jr      z,EndRotMask_double
        ld      b,a
RotRightMask_double:
        rl      d   ;; carry is clear because of "or a" and "dec a"
        rl      e
        rl      c
        djnz    RotRightMask_double
EndRotMask_double:
	ld	a,d
        cpl
        ld      (mask1_double),a
	ld	a,e
        cpl
        ld      (mask2_double),a
        ld      a,c
        cpl
        ld      (mask3_double),a

	ld	l,(iy+1)  ;; font position
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

        ld      a,(font_page)
	out	(3),a
        ld      a,(font_page_hi)
	out	(4),a

        ld      d,(iy)
        inc     iy
        ld      e,(iy)
        ld      c,0

        exx
        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a
        exx

	ld	a,b
	or	a
        jr      z,NoCharacterShift_double
ShiftByte_double:
        rl      d
        rl      e
        rl      c
        djnz    ShiftByte_double
NoCharacterShift_double:
	ld	a,(hl) ;; get from screen
        and     00     ;; self-modifying code  ;; should be AND
mask1_double equ $-1          ;;
	or	d
	ld	(hl),a
	inc	hl
	ld	a,(hl)
        and     00     ;; self-modifying code  ;; should be AND
mask2_double equ $-1          ;;
	or	e
	ld	(hl),a
        inc     hl
	ld	a,(hl)
        and     00     ;; self-modifying code  ;; should be AND
mask3_double equ $-1          ;;
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
        exx
        push    hl
        exx
        pop     de

        ld      c,(iy+2)   ; x-position
        ld      b,(iy+3)
DoCountLoop:
        ld      a,(de)       ;; get character from string
	or	a
        jr      z,doneCounting
        push    bc           ;; x-pos
        ld      hl,(font_len_offset)
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
        add     hl,bc
	ld	a,(font_page)
	out	(3),a
        ld      a,(font_page_hi)
        out     (4),a
	ld	a,(hl)	; width
        ld      l,a
        ld      h,b     ; h=0 as b=0 still
        pop     bc      ; x-position
	add	hl,bc
        ld      c,l
        ld      b,h
	inc	de
        ld      a,7
	out	(3),a
        ld      a,4
	out	(4),a

        jp      DoCountLoop

doneCounting:
        ld      l,c
        ld      h,b
        jp      cleanup_page


Masks:  defb 1,3,7,15,31,63,127,255

ozputchbuf:
        defb 0
        defb 0

; int ozputs(int x, int y, char c);
_ozputch:
        ld   hl,6
        add  hl,sp
        ld   a,(hl)
        ld   (ozputchbuf),a
        ld   bc,ozputchbuf
        ld   (hl),c
        inc  hl
        ld   (hl),b
        jp   _ozputs

psect       bss
init:       defs 1

