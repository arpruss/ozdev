global the_font,_xorcursor,$swapupperpages,_height

psect midpage

osEnableKeyboard equ 0c00fh


global _screenidnum
_screenidnum:
IDNUM: DEFW      0
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
        ld      bc,6*30-30
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
        ld      ix,the_font+32*6
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
        djnz    check_main_nonrev_loop
        ld      ix,the_font+32*6
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
idnumptr:
        defs 2
saved_7:
        defs 1
retval:
        defs 2

global _ozwritefile
;; ozwritefile(char *file, unsigned len, byte type, unsigned *idnum)
_ozwritefile:
        push    ix
        ld      ix,4
        add     ix,sp
        ld      l,(ix)
        ld      h,(ix+1)
        ld      (file),hl
        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      (len),hl
        ld      a,(ix+4)
        ld      (type),a
        ld      l,(ix+6)
        ld      h,(ix+7)
        ld      (idnumptr),hl

        di
        call    $swapupperpages

        in      a,(7)
        ld      (saved_7),a
        ld      a,0ffh
        out     (7),a

        ld      hl,(idnumptr)
        push    hl
        ld      a,(type)
        ld      l,a
        push    hl
        ld      hl,(len)
        push    hl
        ld      hl,(file)
        push    hl
        im      1
        ei
        call    __ozwritefile
        ld      (retval),hl
        di
        ld  a,(osEnableKeyboard)
        and 0ffh-6
        ld  (osEnableKeyboard),a
        pop     bc
        pop     bc
        pop     bc
        pop     bc
        call    $swapupperpages
        ld      a,(saved_7)
        out     (7),a
        im      2
        ei
        ld      hl,(retval)
        pop     ix
        ret

__ozwritefile:
        push    ix
        ld      ix,0
        add     ix,sp
        ld      l,(ix+6)   ; len
        ld      h,(ix+7)
        ld      de,-1024   ; size limit
        add     hl,de
        jr      c,file_too_big
        ld      c,(ix+6)   ; len
        ld      b,(ix+7)
        ld      hl,0
        add     hl,sp
        sbc     hl,bc
        ld      sp,hl
        ex      de,hl
        ld      l,(ix+4)   ; file
        ld      h,(ix+5)
        ldir

        ld      hl,0
        add     hl,sp
        push    hl
        ld      c,(ix+6)
        ld      b,(ix+7)   ; length
        push    bc
        ld      e,(ix+8)   ; type
        ld      c,(ix+0ah) ; idnum
        ld      b,(ix+0bh)
        call    AddFile
        ld      sp,ix
        pop     ix
        ret
file_too_big:
        pop     ix
        ld      l,-1
        ret

global _memo1 ;;,_main

_main:
_memo1:
        ld      hl,IDNUM
        push    hl
        ld      e,2
        push    de
        ld      hl,(LENGTH)
        push    hl
        ld      hl,MEMO
        push    hl
        call    _ozwritefile
        pop     hl
        pop     hl
        pop     hl
        pop     hl
        ret

AddFile:  ;; modified from 1.6.2 OS
push    iy
push    bc
push    de
push    ix
ld      ix,0
add     ix,sp
ld      hl,-14h
add     hl,sp
ld      sp,hl

in      a,(3)
ld      c,a
in      a,(4)
ld      b,a

ld      a,4
out     (3),a
xor     a
out     (4),a

ld      d,a
ld      hl,0bf25h
add     hl,de
ld      a,(hl)
LD      HL,1
ADD     HL,SP

ld      (hl),a
inc     hl
ld      (hl),0       ;; (sp+1)(sp+2) = (400:0bf25h + E)0.

ld      a,c
out     (3),a
ld      a,b
out     (4),a

LD      HL,000Bh
ADD     HL,SP
ld      c,e
ex      de,hl
ld      l,c
ld      h,0
add     hl,hl
add     hl,hl
add     hl,hl   ; hl = 8*E
ld      bc,0c088h
add     hl,bc   ; hl = c088+8*e
ld      bc,8
ldir

LD      C,(IX+02)           ; C = Reg E from call
LD      B,00
LD      HL,TransTable
ADD     HL,BC
LD      C,(HL)        ; (b=0)
LD      (IX-0Fh),C
LD      (IX-0Eh),B

LD      L,(IX-13h)
LD      H,(IX-12h)
ADD     HL,HL
PUSH    HL
LD      HL,000Dh
ADD     HL,SP
LD      C,L
LD      B,H
POP     HL
ADD     HL,BC
LD      B,(HL)
INC     HL
LD      H,(HL)
LD      L,B
LD      (IX-11h),L
LD      (IX-10h),H

LD      L,(IX+04)           ; HL = Pointer to IDNUM
LD      H,(IX+05)
LD      A,(HL)
INC     HL
OR      (HL)
JR      Z,IDNUM_zero         ; Jump if IDNUM = 0000 (New?)

LD      HL,0007
ADD     HL,SP
PUSH    HL
LD      L,(IX+04)           ; HL = Pointer to IDNUM
LD      H,(IX+05)
LD      C,(HL)               ; BC = Value of IDNUM
INC     HL
LD      B,(HL)
LD      E,(IX-11h)
LD      D,(IX-10h)
CALL    07F79h
POP     HL

LD      L,(IX-0Dh)
LD      H,(IX-0Ch)
LD      (IX-0Bh),L
LD      (IX-0Ah),H
JR      Skip_zero

IDNUM_zero:
XOR     A
LD      (IX-0Dh),A
LD      (IX-0Ch),A

Skip_zero:
LD      HL,001Eh
ADD     HL,SP
LD      A,(HL)
ADD     A,0Dh
LD      (HL),A
JR      NC,NoCarry
INC     HL
INC     (HL)
NoCarry:
LD      L,(IX+0Ch)
LD      H,(IX+0Dh)
PUSH    HL
LD      L,(IX+0Ah)           ; HL = Pushed Arg 2 from call
LD      H,(IX+0Bh)
PUSH    HL
LD      HL,000Bh
ADD     HL,SP
PUSH    HL
LD      C,(IX-0Fh)
LD      HL,0011h
ADD     HL,SP
LD      E,L
LD      D,H
CALL    07F73h
POP     HL
POP     HL
POP     HL
LD      (IX-14h),A
OR      A
JR      NZ,GetOut

LD      L,(IX+04h)           ; HL = Pointer to IDNUM
LD      H,(IX+05h)
PUSH    HL
LD      C,(IX-0Dh)
LD      B,(IX-0Ch)
LD      E,(IX-11h)
LD      D,(IX-10h)
CALL    07f7ch
POP     HL

GetOut:
LD      L,(IX-14h)
ld      sp,ix
pop     ix
pop     de
pop     bc
pop     iy
ret

TransTable:
        defb  01,03,02,02,01,02,01,01


