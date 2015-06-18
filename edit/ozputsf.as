*Include macros.inc

newline_width equ 6
newline_char equ 13
psect   beforebss

        global  ScrCharSet,_ozputsf ;; no system calls
        global  __ozmodel,_ozdetectmodel
global _line_buf
global LowerFontPage2,LowerFontPage1,HigherFontPage1,HigherFontPage2
global __ozfontpointers

__ozfontpointers:
        defw    Font0
        defw    Font1
        defw    Font2
        defw    Font3
        defw    Font1
        defw    Font1

Font0:  ;; PC_LARGE
        defb    0eh
LowerFontPage2  equ $-1
        defb    0
        defw    0ab10h
        defw    0ab10h+310h
        defb    13
        defb    0ffh

Font1:  ;; PC_PLANE:
        defb    0eh
LowerFontPage1  equ $-1
        defb    0
        defw    0a000h
        defw    0a000h+310h
        defb    8
        defb    0ffh

Font2:   ;; EO7LARGE
        defb    0fh
HigherFontPage2 equ $-1
        defb    0
        defw    0ab10h
        defw    0ab10h+310h
        defb    13
        defb    0ffh

Font3:   ;; EO7PLANE:
        defb    0fh
HigherFontPage1 equ $-1
        defb    0
        defw    0a000h
        defw    0a000h+310h
        defb    8
        defb    0ffh

DoInit:
        ld      hl,newline
        ld      bc,10000h-0ab10h-310h
        add     hl,bc
        ld      (newline_record+1),hl

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
_ozputsf:
        ld      a,(init)
        or      a
        jr      z,DoInit
DidInit:
        push    iy
	ld	iy,2
	add	iy,sp
	push	ix

	ld	a,(ScrCharSet)
        cp      6
        jr      c,FontOK
        ld      a,1
FontOK:
        ld      c,a
        or      a
        ld      a,0
        jr      nz,NotLarge
        ld      a,13
NotLarge:
        ld      ($newline_comp),a
        ld      a,c

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
        out     (3),a
        ld      a,(ix+1)
        out     (4),a

        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      (font_len_offset),hl
        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      (font_offset),hl
        ld      (font_offset_double),hl

;;;
        ld      a,(iy+2)
        cp      -1
        jp      z,CountOnly
;;;

        ld      e,(ix+6) ;; height
        ld      a,(ix+7) ;; charset mask
        ld      (charset_mask),a

        ld      a,e
        ld      (height),a
        ld      (height_double),a
        ld      (height_clear),a

        ld      hl,400h-4

        ld      c,1
        out     (c),l
        inc     c
        out     (c),h  ;; page in screen


        ld      l,(iy+2)
        ld      h,0
	add	hl,hl
	ld	c,l
	ld	b,h	 ; bc=2*y
	add	hl,hl	 ; hl=4*y
	add	hl,hl	 ; hl=8*y
	add	hl,hl	 ; hl=16*y
	add	hl,hl	 ; hl=32*y
	sbc	hl,bc	 ; hl=30*y

        ld      bc,08000h
        add     hl,bc

	push	hl
	pop	ix	 ; ix=screen-offset for y

        ld      a,0   ;; first column already done
        call    ClearColumn0


        ld      bc,0     ; x position
_ozputsfx equ $-2
global _ozputsfx
        ld      de,_line_buf

DoPrintLoop:
        ld      a,(de)       ;; get character from string
        and     0ffh
charset_mask equ $-1
        jp      z,done

        push    de           ;; string pointer
        push    bc           ;; x-pos

        cp      0
$newline_comp equ $-1
        jp      z,PutNewlineLarge

	ld	hl,0000h     ;; self modifying code
font_len_offset equ $-2
	ld	c,a
	ld	b,0
	add	hl,bc
	add	hl,bc
        add     hl,bc

;; now hl=width, iy=character record position
DoPrintChar:

	push	hl
        pop     iy           ;; iy=font character record position

	ld	a,(hl)	; width
        ld      l,a
        ld      h,b     ; h=0 as b=0 still


        pop     bc      ; x-position
	push	bc

        ld      e,c
        add     hl,bc
        ld      (EndXPos),hl

        cp      9       ; is width more than 8
        jp      nc,DoubleWidth

        ld      a,e     ; low byte of x-position
        and     7
        ld      e,a
        add     a,a
        add     a,e

        ld      e,a
        ld      d,0

        ld      hl,NoRotation
        sbc     hl,de
        ld      (RotateBarrelLoc),hl

	ld	l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset equ $-2	  ;;
        add     hl,bc     ;; hl=position of font data for character

	push	hl
	pop	iy	  ;; iy=font data pointer

	pop	bc	  ;; x-position of start

        srl     c
        srl     c
        srl     c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

        inc     hl
        ld      a,c
        inc     a
        cp      30
        call    c,ClearColumn
        dec     hl

	ld	c,00  ;; character row counter (self-modifying)
height equ $-1	      ;;

InnerCharLoop:
        ld      e,0
        ld      a,(iy)

        jp      0000
RotateBarrelLoc equ $-2

        rla
        rl      e

        rla
        rl      e

        rla
        rl      e

        rla
        rl      e

        rla
        rl      e

        rla
        rl      e

        rla
        rl      e

NoRotation:
        or      (hl) ;; get from screen
	ld	(hl),a
	inc	hl
        ld      a,(hl)
        or      e
        ld      (hl),a
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

        jp      DoPrintLoop

done:
        push    ix
        pop     hl          ;; Position

        push    bc

        ld      a,(already_did)
        inc     a
        ld      c,a

        add     hl,bc


FinalClearTop:
        cp      30
        jp      nc,cleanup_page0
        call    ClearColumn
        inc     hl
        inc     a
        jp      FinalClearTop

cleanup_page0:
        pop     bc
cleanup_page:
	pop	ix
	pop	iy

        ld      l,c

        ld      a,7
        out     (3),a
        ld      a,6-4
        out     (1),a
        ld      a,4
        out     (2),a
        out     (4),a

	ret

DoubleWidth:
        ld      a,e     ; low byte of x-position
        and     7
        ld      (low_nibble_double),a

	ld	l,(iy+1)  ;; font position
	ld	h,(iy+2)
	ld	bc,0000h  ;; self-modifying code
font_offset_double equ $-2       ;;
        add     hl,bc     ;; hl=position of font data for character
	push	hl
	pop	iy	  ;; iy=font data pointer

	pop	bc	  ;; x-position of start

        srl     c
        srl     c
        srl     c
	ld	b,0
	push	ix
	pop	hl	  ;; hl=screen position
	add	hl,bc

        inc     hl
        ld      a,c
        inc     a
        cp      30
        call    c,ClearColumn
        inc     hl
        inc     a
        cp      30
        call    c,ClearColumn
        dec     hl
        dec     hl

        push    ix

        undoc_ix
        ld      l,00  ;; ld ixl,00 : character row counter (self-modifying)
height_double equ $-1        ;;

InnerCharLoop_double:
        ld      b,00  ;; self-modifying code
low_nibble_double equ $-1    ;;

        inc     iy
        ld      e,(iy)
        ld      c,0

	ld	a,b
	or	a
        ld      a,(iy-1)
        jr      z,NoCharacterShift_double
ShiftByte_double:
        rla
        rl      e
        rl      c
        djnz    ShiftByte_double
NoCharacterShift_double:
        or      (hl) ;; get from screen
	ld	(hl),a
	inc	hl
	ld	a,(hl)
        or      e
	ld	(hl),a
        inc     hl
	ld	a,(hl)
        or      c
        ld      (hl),a
        ld      de,28
	add	hl,de
	inc	iy
        undoc_ix
        dec     l   ;; dec ixl
        jp      nz,InnerCharLoop_double
        pop     ix
        jp      EndOfCharPut

PutNewlineLarge:
        ld      hl,newline_record
        jp      DoPrintChar

ClearColumn:
        cp      0
already_did equ $-1
        ret     z
        ret     c
ClearColumn0:
        ld      (already_did),a
        nop
_skipclearret equ $-1
global _skipclearret
        ex      af,af'
        push    hl
        ld      de,30
        ld      b,0
height_clear equ $-1
        xor     a
CCTop:
        ld      (hl),a
        add     hl,de
        djnz    CCTop

        pop     hl
        ex      af,af'
        ret

;;; added
CountOnly:
        ld      de,_line_buf
        ld      hl,0
        ld      b,0

CountTop:
        ld      c,l
        ld      a,(de)
        or      a
        jp      z,cleanup_page
        inc     de
        push    hl
        ld      c,a
        ld      hl,(font_len_offset)
        add     hl,bc
        add     hl,bc
        add     hl,bc

        ld      c,(hl)
        pop     hl
        add     hl,bc
        jr      CountTop

;;; added end


;Masks:  defb 1,3,7,15,31,63,127,255
init:   defb 0

newline_record:
defb 6
defw 0

newline:
defb 0h,0h,10h,10h,10h,10h,14h,12h,1Fh,2h,4h,0,0

;_line_buf:
;        defm 'H'
;        defb 0
;_check: defw 0
;global _check
