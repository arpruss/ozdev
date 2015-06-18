WIDTH   equ 60

        psect text
global _beep
_beep:
        xor     a
	out	(19h),a
	ld	a,20h
	out	(15h),a
	ret

;; NOTE: highpage must load at or after c000h
;        psect highpage
;screen_swap_space:
;        defs  2400
;

        psect lowpage

;global  _save_screen
;_save_screen:
;        in      a,(3)
;        ld      c,a
;        in      a,(4)
;        ld      b,a
;        push    bc
;        ld      bc,0400h
;        ld      a,c
;        out     (3),a
;        ld      a,b
;        out     (4),a
;        ld      hl,0a000h
;        ld      de,screen_swap_space
;finish_save:
;        ld      bc,2400
;        ldir
;        pop     bc
;        ld      a,c
;        out     (3),a
;        ld      a,b
;        out     (4),a
;        ret
;
;global _restore_screen
;_restore_screen:
;        in      a,(3)
;        ld      c,a
;        in      a,(4)
;        ld      b,a
;        push    bc
;        ld      bc,0400h
;        ld      a,c
;        out     (3),a
;        ld      a,b
;        out     (4),a
;        ld      hl,screen_swap_space
;        ld      de,0a000h
;        jr      finish_save


global  _reverse,_underline
_reverse:
        defb 0
_underline:
        defb 0

global _ttyclearregion
; void ttyclearregion(unsigned from,unsigned numbytes,
; unsigned firstmask,unsigned lastmask,unsigned numlines);
_ttyclearregion:
        push    ix
        ld      ix,4
        add     ix,sp
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      l,(ix)
        ld      h,(ix+1)     ; from
        ld      de,0a000h
        add     hl,de
        ld      e,(ix+8)     ; numlines
doline:
        push    hl
        ld      c,(ix+2)     ; numbytes
        ld      a,(ix+4)     ; firstmask
        and     (hl)
        ld      (hl),a
        xor     a
        dec     c
        cp      c
        jp      z,linedone
clearcols:
        inc     hl
        dec     c
        cp      c
        jp      z,dolastcol
        ld      (hl),a
        jp      clearcols
dolastcol:
        ld      a,(ix+6)     ; lastmask
        and     (hl)
        ld      (hl),a
linedone:
        xor     a
        dec     e
        cp      e
        pop     hl
        jp      z,cleardone
        ld      bc,30
        add     hl,bc
        jp      doline
cleardone:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        pop     ix
        ret

global _scrolltop,_scrolllen,_scrollbotline,_smoothscroll

_smoothscroll:
        defb 0
_scrolltop:
        defw 0a000h
_scrolllen:
        defw 2160
_scrollbotline:
        defw 0a000h+2160

global _ttyscroll
_ttyscroll:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      de,(_scrolltop)

        ld      a,(_smoothscroll)
        or      a
        jp      nz,smoothscroll

        ld      hl,180
_word_180_150  equ $-2
global _word_180_150
        add     hl,de
        ld      bc,(_scrolllen)
        ldir
        ld      l,e
        ld      h,d
        inc     de
        ld      bc,180-1
_word_179_149  equ $-2
global _word_179_149
	ld	(hl),0
        ldir

scroll_cleanup:
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret
smoothscroll:
        ld      hl,30
        add     hl,de
        push    hl
        ld      hl,(_scrolllen)
        ld      bc,5*30
_word_150_120   equ $-2
global  _word_150_120
        add     hl,bc
        ld      c,l
        ld      b,h
        pop     hl
        push    bc
        push    de
        push    hl
        call    smoothscroll1
        call    delay
        pop     hl
        pop     de
        pop     bc
        push    bc
        push    de
        push    hl
        call    smoothscroll1
        call    delay
        pop     hl
        pop     de
        pop     bc
        push    bc
        push    de
        push    hl
        call    smoothscroll1
        call    delay
        pop     hl
        pop     de
        pop     bc
        push    bc
        push    de
        push    hl
        call    smoothscroll1
        call    delay
        pop     hl
        pop     de
        pop     bc
        push    bc
        push    de
        push    hl
        call    smoothscroll1
        call    delay
        pop     hl
        pop     de
        pop     bc
        ld      a,0  ;; self-mod
_byte_0_1       equ $-1
global _byte_0_1
        or      a
        jr      nz,scroll_cleanup
        call    smoothscroll1
        jp      scroll_cleanup

delay:
        push     hl
        ld       hl,2000
rpt:
        dec      hl
        ld       a,l
        or       h
        jr       nz,rpt
        pop      hl
        ret

smoothscroll1:
        ldir
        ld      l,e
        ld      h,d
        inc     de
        ld      (hl),0
        ld      bc,30-1
        ldir
        ret

global _ttyscrolldown
_ttyscrolldown:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

        ld      hl,(_scrollbotline)
        dec     hl
        ld      d,h
        ld      e,l
        ld      bc,180
_word_180_150_2 equ $-2
global _word_180_150_2
        add     hl,bc
        ex      de,hl
        ld      bc,(_scrolllen)
        lddr
        ld      hl,(_scrolltop)
        ld      e,l
        ld      d,h
        inc     de
        ld      bc,180-1
_word_179_149_2 equ $-2
global _word_179_149_2
	ld	(hl),0
        ldir

        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

_curcol:
        defb 0
_currow:
        defb 0

global _xorcursor,_curcol,_currow

_fix_xor:
        push    af
        ld      a,WIDTH-1
        ld      (_curcol),a
        call    _xorcursor0
        pop     af
        ld      (_curcol),a
        ret
_xorcursor:
        ld      a,(_curcol)
        cp      WIDTH
        jp      z,_fix_xor
_xorcursor0:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc
        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

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
_word_2919h_1929h:
global _word_2919h_1929h
	add	hl,de
	add	hl,hl
	add	hl,hl
        ld      d,0a0h
	ld	e,b
	srl	e
	add	hl,de
        ld      de,5*30
_word_150_120_2 equ $-2
global _word_150_120_2
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
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

global _ttyprint
; ttyprint(char c)
_ttyprint:
        ld      a,(_curcol) ; col
        cp      WIDTH       ; wrap text immediately
_the_width equ $-1
global _the_width
global  _crlf
        call    nc,_crlf

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

        push    ix

        ld      hl,6
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

        ld      a,(_reverse)
        or      a
        jp      nz,do_reverse
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
        ld      e,a
        ld      a,(_underline)
        or      a
        ld      a,e
        jp      z,nounderline
        xor     0fh
nounderline:
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

        ld      e,a
        ld      a,(_underline)
        or      a
        ld      a,e
        jp      z,nounderline2
        xor     0f0h
nounderline2:
        ld      (hl),a
        ld      a,(_curcol)
        ld      e,a
        cp      WIDTH-1
        jp      z,LastColumn_delund
        inc     hl
        ld      a,(hl)
        xor     7h
        ld      (hl),a

cursor:
        ld      a,e
        inc     a
        ld      (_curcol),a
        pop     ix
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

LastColumn_delund:
        ld      a,(hl)
        and     7fh     ;; delete any bleeding underline
        xor     70h
        ld      (hl),a
        jp      cursor

LastColumn:
        ld      a,(hl)
        xor     70h
        ld      (hl),a
        jp      cursor

do_reverse:
        bit     0,c
        ld      bc,30
        jp      nz,rchar1

rchar0:
        ld      d,0fh          ; big unrolled loop for even chars
        ld      a,d
        or      (hl)
        xor     (ix)
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+1)
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+2)
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+3)
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+4)
_nop_start_3:
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+5)
_nop_end_3:
global _nop_start_3,_nop_end_3
        ld      e,a
        ld      a,(_underline)
        or      a
        ld      a,e
        jp      z,rnounderline
        xor     d
rnounderline:
        xor     70h             ; put cursor to the right
        ld      (hl),a
        ld      a,(_curcol)
        ld      e,a
        jp      cursor

rchar1:  ld      a,(ix)          ; big unrolled loop for odd chars
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      d,0f0h
        ld      a,d
        or      (hl)
        xor     e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+1)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+2)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+3)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+4)
_nop_start_4:
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+5)
_nop_end_4:
global  _nop_start_4,_nop_end_4
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e

        ld      e,a
        ld      a,(_underline)
        or      a
        ld      a,e
        jp      z,rnounderline2
        xor     d
rnounderline2:
        ld      (hl),a
        ld      a,(_curcol)
        ld      e,a
        cp      WIDTH-1
        jp      z,LastColumn
        inc     hl
        ld      a,(hl)
        xor     7h
        ld      (hl),a
        jp      cursor

global the_font
the_font:
*Include font.as
