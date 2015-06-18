global the_font,_xorcursor,$swapupperpages,_height

psect text

osEnableKeyboard equ 0c00fh

HEADLENGTH equ MEMOHEAD_END-MEMO
MEMO:
       ;; First 13 bytes data seems to be a kind of work area.
       ;; More investigation required...
       DefB      'M','E','M','O',0ffh,0ffh,0d0H  ;; change D0 to 50h
       DefB      0dh,00H,16H,00H,00H,00H       ;; for a secret memo
       ;; Date in yyyymmdd + (terminating zero)
       Defm      '20000801'
       defb 0H
       ;; Title string.  This is a fixed-length field (21 bytes)
       Defm      'Wizterm Capture  '
       defb 20H, 20H, 20H, 0H
       ;; Memo body.  This is a variable-length field
MEMOHEAD_END:
screenbuf:
        defs 16*60+16+1
screenbuf_ptr:
        defs 2
global _screenmemolen
_screenmemolen:
LENGTH: defs 2

psect lowpage
line:
        defs    60
        defb    00

; char *scan_screen(void)
global _scan_screen
_scan_screen:
        call    _xorcursor
        push    ix
        push    iy
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc
        ld      hl,screenbuf
        ld      (screenbuf_ptr),hl
        ld      hl,0a000h
        ld      a,(_height)
        ld      c,a
outerlinelooptop:
        push    bc
        pop     iy

        xor     a
        out     (3),a
        ld      a,4         ;; switch to screen page
        out     (4),a

        ld      b,30
        ld      de,line
innerlinelooptop:

        push    bc
        push    de
        push    hl
        call    get_even_char
        pop     hl
        pop     de
;        pop     bc

        ld      (de),a
        inc     de

;        push    bc
        push    de
        push    hl
;        ld      e,07fh   ;; mask for last line
;        ld      a,b
;        cp      1
;        jp      z,NoChangeMask
;        ld      e,0ffh
;NoChangeMask:
        call    get_odd_char
        pop     hl
        pop     de
        pop     bc
        ld      (de),a
        inc     de
        inc     hl
        djnz    innerlinelooptop

        push    hl
        pop     ix

        pop     bc
        push    bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      b,60
        ld      hl,line+60-1
scanspace:
        ld      a,(hl)
        cp      ' '
        jr      nz,NonSpace
        ld      (hl),0
        dec     hl
        djnz    scanspace
NonSpace:
        ld      hl,line
        ld      de,(screenbuf_ptr)
copyline:
        ld      a,(hl)
        or      a
        jr      z,end_copyline
        ld      (de),a
        inc     hl
        inc     de
        jp      copyline
end_copyline:
        ld      a,0dh
        ld      (de),a
        inc     de
        ld      (screenbuf_ptr),de
        push    ix
        pop     hl
        ld      bc,8*30-30
_word_150_120_3 equ $-2
global _word_150_120_3
        add     hl,bc
        push    iy
        pop     bc
        dec     c
        jr      nz,outerlinelooptop

        ld      hl,(screenbuf_ptr)
        ld      b,0
        ld      (hl),b
        dec     hl

        ld      a,0dh
ZeroEOLs:
        cp      (hl)
        jr      nz,DoneZeroingEOLs
        ld      (hl),b
        dec     hl
        jr      ZeroEOLs
DoneZeroingEOLs:
        xor     a
        cp      (hl)
        jr      z,null_screen
        inc     hl
        inc     hl
        ld      bc,MEMO
        or      a
        sbc     hl,bc
        ld      (LENGTH),hl
GetOutOfCopy:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     iy
        pop     ix
        call    _xorcursor
        ld      hl,MEMO
        ret

null_screen:
        ld      hl,screenbuf
        ld      (hl),'.'
        inc     hl
        ld      (hl),0
        ld      hl,screenbuf+2-MEMO
        ld      (LENGTH),hl
        jr      GetOutOfCopy

psect lowpage
get_even_char:
;; scan character at (hl), comparing against the font at the_font
        ld      ix,the_font+32*8
        ld      b,128-32
check_main_nonrev_loop:
        push    bc
        push    hl
        call    check_even_nonrev
        pop     hl
        pop     bc
        jp      z,Found
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        djnz    check_main_nonrev_loop
        ld      ix,the_font+32*8
        ld      b,128-32
check_main_rev_loop:
        push    bc
        push    hl
        call    check_even_rev
        pop     hl
        pop     bc
        jp      z,Found
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        djnz    check_main_rev_loop

        ld      a,'.'
        ret

Found:
        ld      a,128
        sub     b
        ret

check_even_nonrev:
        ld      a,(hl)
        and     0fh
        cp      (ix)
        ret     nz

        ld      bc,30

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        cp      (ix+1)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        cp      (ix+2)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        cp      (ix+3)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        cp      (ix+4)
;        ret     nz
;
;        add     hl,bc
;        ld      a,(hl)   ;; load from font
;        and     0fh
;        cp      (ix+5)
        ret

check_even_rev:
        ld      a,(hl)
        and     0fh
        xor     0fh
        cp      (ix)
        ret     nz

        ld      bc,30

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        xor     0fh
        cp      (ix+1)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        xor     0fh
        cp      (ix+2)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        xor     0fh
        cp      (ix+3)
        ret     nz

        add     hl,bc
        ld      a,(hl)   ;; load from font
        and     0fh
        xor     0fh
        cp      (ix+4)
;        ret     nz
;
;        add     hl,bc
;        ld      a,(hl)   ;; load from font
;        and     0fh
;        cp      (ix+5)
        ret


get_odd_char:
;; scan character at (hl), comparing against the font at the_font
;        ld      a,e
;        ld      (mask),a
        ld      ix,the_font+32*6
        ld      b,128-32
check_odd_nonrev_loop:
        push    bc
        push    hl
        call    check_odd_nonrev
        pop     hl
        pop     bc
        jp      z,Found
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        djnz    check_odd_nonrev_loop

        ld      ix,the_font+32*6
        ld      b,128-32
check_odd_rev_loop:
        push    bc
        push    hl
        call    check_odd_rev
        pop     hl
        pop     bc
        jp      z,Found
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        inc     ix
        djnz    check_odd_rev_loop

        ld      a,'.'
        ret

check_odd_nonrev:
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        cp      (ix)
        ret     nz

        ld      bc,30

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        cp      (ix+1)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        cp      (ix+2)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        cp      (ix+3)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        cp      (ix+4)
;        ret     nz
;
;        add     hl,bc
;        ld      a,(hl)
;        and     0ffh     ;; self-mod
;mask    equ     $-1      ;;
;        srl     a
;        srl     a
;        srl     a
;        srl     a
;        cp      (ix+5)
        ret

check_odd_rev:
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        xor     0fh
        cp      (ix)
        ret     nz

        ld      bc,30

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        xor     0fh
        cp      (ix+1)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        xor     0fh
        cp      (ix+2)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        xor     0fh
        cp      (ix+3)
        ret     nz

        add     hl,bc
        ld      a,(hl)
        srl     a
        srl     a
        srl     a
        srl     a
        xor     0fh
        cp      (ix+4)
;        ret     nz
;
;        add     hl,bc
;        ld      a,(hl)
;        and     0ffh     ;; self-mod
;mask    equ     $-1      ;;
;        srl     a
;        srl     a
;        srl     a
;        srl     a
;        xor     0fh
;        cp      (ix+5)
        ret


file:
        defs 2
len:
        defs 2
type:
        defs 1
saved_7:
        defs 1
retval:
        defs 2


