WIDTH   equ 60

        psect text

global set_a000_400, restore_a000

global _ttyscroll
_ttyscroll:
        call    set_a000_400

        ld      de,0a000h

        ld      hl,180
        add     hl,de
        ld      bc,2160
        ldir
        ld      l,e
        ld      h,d
        inc     de
        ld      bc,180-1
	ld	(hl),0
        ldir

        jp      restore_a000

_curcol:
        defb 0
_currow:
        defb 0

global _xorcursor,_curcol,_currow

_xorcursor:
        call    set_a000_400

        ld      a,(_curcol) ; col
        ld      b,a
        ld      a,(_currow) ; row
        ld      c,a        ; calculate screen offset, result in hl
	sla	a
	sla	a
	sla	a
	sla	a
	sub	c
	ld	l,a
	ld	h,0
	ld	d,h
	ld	e,l
	add	hl,hl
	add	hl,de
	add	hl,hl
	add	hl,hl
        ld      d,0a0h
	ld	e,b
	srl	e
	add	hl,de
        ld      de,5*30
        add     hl,de
        bit     0,b
        ld      a,(hl)
        jp      nz,cursor1
        xor     7h
        jp      cursor_done
cursor1:
        xor     070h
cursor_done:
        ld      (hl),a

        jp      restore_a000

global _ttyprint
; ttyprint(char c)
_ttyprint:
        ld      a,(_curcol) ; col
        cp      WIDTH       ; wrap text immediately
_the_width equ $-1
global _the_width
global  _crlf
        call    nc,_crlf

        call    set_a000_400

        push    ix

        ld      hl,4
        add     hl,sp
        ld      l,(hl)          ; print char
        ld      h,0

        add     hl,hl  ; hl=char*2   ;11
        ld      c,l                  ;4
        ld      b,h                  ;4
        add     hl,bc  ; hl=char*4   ;11
        add     hl,bc  ; hl=char*6   ;11
        ex      de,hl                ;4
        ld      ix,the_font              ;14
        add     ix,de                ;15  ;; ix=the_font+char*6

        ld      hl,_curcol
        ld      c,(hl)
        inc     hl
        ld      a,(hl) ; _currow
        ld      b,a
                     ; calculate screen offset, result in hl
	sla	a
	sla	a
	sla	a
	sla	a
        sub     b
	ld	l,a
	ld	h,0
	ld	d,h
	ld	e,l
	add	hl,hl
_word_2919h_1929h_2:
global _word_2919h_1929h_2
	add	hl,de
	add	hl,hl
	add	hl,hl
        ld      d,0a0h
        ld      e,c
	srl	e
	add	hl,de

        bit     0,c
        ld      bc,30
        jp      nz,char1

char0:  ld      e,0f0h
        ld      a,e          ; big unrolled loop for even chars
	and	(hl)
	or	(ix)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+1)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+2)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+3)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+4)
_nop_start:
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+5)
_nop_end:
global _nop_start,_nop_end
        xor     70h             ; put cursor to the right
        ld      (hl),a
        ld      a,(_curcol)
        ld      e,a

	jp	cursor

char1:	ld	a,(ix)		; big unrolled loop for odd chars
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      d,0fh
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+1)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+2)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+3)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+4)
_nop_start_2:
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+5)
_nop_end_2:
global _nop_start_2,_nop_end_2
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
        add     a,e

        ld      (hl),a
        ld      a,(_curcol)
        ld      e,a
        cp      WIDTH-1
        inc     hl
        ld      a,(hl)
        xor     7h
        ld      (hl),a

cursor:
        ld      a,e
        inc     a
        ld      (_curcol),a
        pop     ix
        jp      restore_a000

LastColumn:
        ld      a,(hl)
        xor     70h
        ld      (hl),a
        jp      cursor


global the_font
the_font:
*Include font.as
