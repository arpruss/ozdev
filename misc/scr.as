WIDTH   equ 59

        psect text
global _beep
_beep:
        xor     a
	out	(19h),a
	ld	a,20h
	out	(15h),a
	ret

        psect lowpage

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
        defw 0a870h

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

        ld      a,(_scrolltop)
        ld      e,a
        ld      a,(_scrolltop+1)
        ld      d,a

        ld      a,(_smoothscroll)
        or      a
        jp      nz,smoothscroll

        ld      hl,0b4h
        add     hl,de
        ld      a,(_scrolllen)
        ld      c,a
        ld      a,(_scrolllen+1)
        ld      b,a
        ldir
        ld      l,e
        ld      h,d
        inc     de
        ld      bc,180-1
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
        ld      a,(_scrolllen)
        ld      l,a
        ld      a,(_scrolllen+1)
        ld      h,a
        ld      bc,5*30
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
        call    smoothscroll1
        jp      scroll_cleanup

delay:
        push     hl
        ld       hl,1000
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

        ld      a,(_scrollbotline)
        ld      l,a
        ld      a,(_scrollbotline+1)
        ld      h,a
        dec     hl
        ld      d,h
        ld      e,l
        ld      bc,0b4h
        add     hl,bc
        ex      de,hl
        ld      a,(_scrolllen)
        ld      c,a
        ld      a,(_scrolllen+1)
        ld      b,a
        lddr
        ld      a,(_scrolltop)
        ld      l,a
        ld      e,a
        ld      a,(_scrolltop+1)
        ld      h,a
        ld      d,a
        inc     de
        ld      bc,180-1
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
_xorcursor:
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
        cp      WIDTH
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
        ld      ix,font              ;14
        add     ix,de                ;15  ;; ix=font+char*6

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
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+5)
        ld      b,a
        ld      a,(_underline)
        or      a
        ld      a,b
        jp      z,nounderline
        xor     0fh
nounderline:
        xor     70h             ; put cursor to the right
        ld      (hl),a

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
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
        add     a,e

        ld      b,a
        ld      a,(_underline)
        or      a
        ld      a,b
        jp      z,nounderline2
        xor     0f0h
nounderline2:
        ld      (hl),a
        inc     hl
        ld      a,(hl)
        xor     7h
        ld      (hl),a

cursor:
        ld      hl,_curcol
        inc     (hl)

        pop     ix
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

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
	ld	(hl),a
        add     hl,bc
        ld      a,d
        or      (hl)
        xor     (ix+5)
        ld      b,a
        ld      a,(_underline)
        or      a
        ld      a,b
        jp      z,rnounderline
        xor     d
rnounderline:
        xor     70h             ; put cursor to the right
        ld      (hl),a

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
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
        or      (hl)
        xor     e

        ld      b,a
        ld      a,(_underline)
        or      a
        ld      a,b
        jp      z,rnounderline2
        xor     d
rnounderline2:
        ld      (hl),a
        inc     hl
        ld      a,(hl)
        xor     7h
        ld      (hl),a
        jp      cursor

font:
defb 0,0,0,0,0,0,0,5,0,5
defb 2,0,0,5,0,0,7,0,0,5
defb 7,7,2,0,0,2,7,7,2,0
defb 6,15,15,6,15,0,2,7,2,2
defb 7,0,0,0,0,0,0,0,15,15
defb 9,9,15,15,0,0,0,0,0,0
defb 0,0,0,0,0,0,0,0,0,0
defb 0,0,0,0,0,0,0,0,0,0
defb 0,0,0,0,14,10,10,10,5
defb 0,9,6,9,9,6,9,1,3,7,3
defb 1,0,4,6,7,6,4,0,2,7
defb 2,2,7,2,5,5,5,0,5,0
defb 14,11,11,10,10,0,14,1,6
defb 6,8,7,0,0,0,15,15,0,2
defb 7,2,7,2,15,2,7,2,2,2
defb 2,2,2,2,2,7,2,0,4,15
defb 4,0,0,0,2,15,2,0,0,0
defb 0,0,0,0,0,0,0,0,0,0
defb 0,0,0,0,0,0,0,0,0,0
defb 0,0,0,0,0,0,0,0,0,1
defb 1,1,0,1,0,5,5,0,0,0
defb 0,6,15,6,15,6,0,6,3,7
defb 6,3,0,1,4,2,1,4,0,2
defb 5,2,5,10,0,2,2,1,0,0
defb 0,4,2,2,2,4,0,1,2,2
defb 2,1,0,5,2,7,2,5,0,0
defb 2,7,2,0,0,0,0,0,2,1
defb 0,0,0,7,0,0,0,0,0,0
defb 0,1,0,4,2,2,2,1,0,2
defb 5,5,5,2,0,2,3,2,2,2
defb 0,3,4,2,1,7,0,3,4,3
defb 4,3,0,4,6,5,7,4,0,7
defb 1,3,4,3,0,6,1,3,5,2
defb 0,7,4,2,2,2,0,2,5,2
defb 5,2,0,2,5,6,4,2,0,0
defb 2,0,2,0,0,0,2,0,2,1
defb 0,4,2,1,2,4,0,0,7,0
defb 7,0,0,1,2,4,2,1,0,3
defb 4,2,0,2,0,6,9,13,13,1
defb 6,2,5,7,5,5,0,3,5,3
defb 5,3,0,6,1,1,1,6,0,3
defb 5,5,5,3,0,7,1,3,1,7
defb 0,7,1,3,1,1,0,6,1,5
defb 5,6,0,5,5,7,5,5,0,7
defb 2,2,2,7,0,7,2,2,2,1
defb 0,5,5,3,5,5,0,1,1,1
defb 1,7,0,5,7,7,5,5,0,3
defb 5,5,5,5,0,2,5,5,5,2
defb 0,3,5,3,1,1,0,2,5,5
defb 3,6,0,3,5,3,5,5,0,6
defb 1,2,4,3,0,7,2,2,2,2
defb 0,5,5,5,5,6,0,5,5,5
defb 5,2,0,5,5,7,7,5,0,5
defb 5,2,5,5,0,5,5,2,2,2
defb 0,7,4,2,1,7,0,6,2,2
defb 2,6,0,1,2,2,2,4,0,3
defb 2,2,2,3,0,2,5,0,0,0
defb 0,0,0,0,0,0,15,2,2,4
defb 0,0,0,0,3,6,5,6,0,1
defb 3,5,5,3,0,0,6,1,1,6
defb 0,4,6,5,5,6,0,0,2,5
defb 3,6,0,4,2,7,2,2,0,0
defb 6,5,6,3,0,1,3,5,5,5
defb 0,2,0,2,2,2,0,4,0,4
defb 4,2,0,1,1,5,3,5,0,2
defb 2,2,2,4,0,0,3,7,7,5
defb 0,0,3,5,5,5,0,0,2,5
defb 5,2,0,0,3,5,3,1,0,0
defb 6,5,6,12,0,0,3,5,1,1
defb 0,0,6,3,6,3,0,2,7,2
defb 2,4,0,0,5,5,5,6,0,0
defb 5,5,5,2,0,0,5,7,7,2
defb 0,0,5,2,2,5,0,0,5,5
defb 6,3,0,0,7,4,2,7,0,4
defb 2,3,2,4,0,2,2,0,2,2
defb 0,1,2,6,2,1,0,10,5,0
defb 0,0,0,0,2,5,7,0,0,6
defb 1,6,4,2,0,5,0,5,5,6
defb 0,6,0,2,7,6,0,2,5,2
defb 5,6,0,5,0,2,5,6,0,3
defb 0,2,5,6,0,2,0,2,5,6
defb 0,0,6,1,6,2,0,2,5,2
defb 7,6,0,5,0,2,7,6,0,3
defb 0,2,7,6,0,5,0,2,2,2
defb 0,2,5,2,2,2,0,3,0,2
defb 2,2,0,5,2,5,7,5,0,2
defb 2,5,7,5,0,6,0,7,3,7
defb 0,0,14,13,5,14,0,14,5
defb 15,5,13,0,7,0,2,5,2,0
defb 5,0,2,5,2,0,3,0,2,5,2
defb 0,2,5,0,5,6,0,3,0,5
defb 5,6,0,5,0,5,6,3,0,9
defb 6,9,9,6,0,5,0,5,5,6
defb 0,2,7,1,7,2,0,6,2,7
defb 2,7,0,5,5,2,7,2,0,3
defb 5,7,13,5,0,6,2,7,2,3
defb 0,6,0,2,5,6,0,6,0,2
defb 2,2,0,6,0,2,5,2,0,6
defb 0,5,5,6,0,7,0,3,5,5
defb 0,7,0,7,5,5,0,2,5,6
defb 0,7,0,2,5,2,0,7,0,2
defb 0,2,1,6,0,0,0,7,1,0
defb 0,0,0,7,4,0,0,5,5,2
defb 7,13,0,5,5,6,15,5,0,1
defb 0,1,1,1,0,0,6,3,6,0
defb 0,0,3,6,3,0,0,5,0,5
defb 0,5,0,5,10,5,10,5,10
defb 15,10,15,10,15,10,2,2,2,2
defb 2,2,2,2,3,2,2,2,2,2
defb 3,3,2,2,6,6,7,6,6,6
defb 0,0,7,6,6,6,0,0,3,3
defb 2,2,6,6,7,7,6,6,6,6
defb 6,6,6,6,0,0,7,7,6,6
defb 6,6,7,7,0,0,6,6,7,0
defb 0,0,2,2,3,3,0,0,0,0
defb 3,2,2,2,2,2,14,0,0,0
defb 2,2,15,0,0,0,0,0,15,2
defb 2,2,2,2,14,2,2,2,0,0
defb 15,0,0,0,2,2,15,2,2,2
defb 2,2,14,14,2,2,6,6,14,6
defb 6,6,6,6,14,14,0,0,0,0
defb 14,14,6,6,6,6,15,15,0
defb 0,0,0,15,15,6,6,6,6,14
defb 14,6,6,0,0,15,15,0,0,6
defb 6,15,15,6,6,2,2,15,15,0
defb 0,6,6,15,0,0,0,0,0,15
defb 15,2,2,0,0,15,6,6,6,6
defb 6,15,0,0,0,2,2,14,14,0
defb 0,0,0,14,14,2,2,0,0,14
defb 6,6,6,6,6,15,6,6,6,2
defb 2,15,15,2,2,2,2,3,0,0
defb 0,0,0,14,2,2,2,15,15
defb 15,15,15,15,0,0,0,15,15,15
defb 3,3,3,3,3,3,12,12,12
defb 12,12,12,15,15,15,0,0,0,0
defb 10,5,5,10,0,2,5,3,5,3
defb 1,7,5,1,1,1,0,0,15,5
defb 5,5,0,7,1,2,1,7,0,0
defb 14,5,5,2,0,0,5,5,3,1
defb 0,0,13,2,2,2,0,7,2,5
defb 2,7,0,2,5,7,5,2,0,2
defb 5,5,2,5,0,6,1,2,5,2
defb 0,0,6,15,6,0,0,8,6,15
defb 6,1,0,6,1,7,1,6,0,2
defb 5,5,5,5,0,7,0,7,0,7
defb 0,2,7,2,0,7,0,3,6,3
defb 0,7,0,6,3,6,0,7,0,6
defb 2,2,2,2,2,2,2,2,2,3
defb 0,2,0,7,0,2,0,10,5,0
defb 10,5,0,2,5,2,0,0,0,0
defb 0,6,6,0,0,0,0,2,0,0
defb 0,14,2,2,3,2,0,0,3,5
defb 5,5,0,3,4,2,7,0,0,0
defb 7,7,7,0,0,0,0,0,0,0,0
