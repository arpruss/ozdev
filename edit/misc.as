SECURE equ 0

;; miscellaneous routines

arrow3 equ 0 ; 7fh
arrow2 equ 3eh
arrow1 equ 1ch
arrow0 equ 8h

psect lowpage

;; global _reverselines
global _scr_down,_scr_up
;;global _gettemplength
;;global _zerotemplength
global _uparrow,_downarrow,_cleararrows
global _stripe
global _ozgetw
global _cleararea,restore_a000,set_a000_400

_scr_down:
        call    set_a000_400

        ld      de,0a000h+2400-1
        ld      hl,0a000h+2400-10*30-1
        ld      bc,6*10*30-30
        lddr

        jp      restore_a000

_scr_up:
        call    set_a000_400

        ld      de,0a000h+11*30
        ld      hl,0a000h+11*30+10*30
        ld      bc,6*10*30-30
        ldir

        jp      restore_a000

_cleararrows:
        call    set_a000_400
        ld      hl,0a000h+29
        ld      de,30
        ld      b,7
        xor     a
catop:
        ld      (hl),a
        add     hl,de
        djnz    catop
        jp      restore_a000

_uparrow:
        ld      hl,0a000h+29
        ld      de,30
arrow_do:
        call    set_a000_400 ;; only affects accumulator
        ld      a,0010000B
        ld      (hl),a
        add     hl,de
        ld      a,0111000B
        ld      (hl),a
        add     hl,de
        ld      a,1111100B
        ld      (hl),a
        jp      restore_a000

_downarrow:
        ld      hl,0a000h+29+30*6
        ld      de,-30
        jr      arrow_do

_stripe:
        call set_a000_400
        ld      hl,0a000h+22+11*30
        ld      c,2
        ld      de,60
        ld      b,35
STop:
        ld      a,(hl)
        or      c
        ld      (hl),a
        add     hl,de
        djnz    STop
        jp      restore_a000

_cleararea:
;; void cleararea(unsigned start, unsigned count);
        call set_a000_400

        pop     de
        pop     hl
        pop     bc
        push    bc
        push    hl
        push    de
        
        ld      de,0a000h
        add     hl,de
        ld      (hl),e
        dec     bc
        ld      e,l
        ld      d,h
        inc     de
        ldir

        jp      restore_a000

psect text
global _ozreadfilebyte
_ozgetw:
        call _ozreadfilebyte
        push    hl
        call    _ozreadfilebyte
        pop     bc
        ld      h,l
        ld      l,c
        ret

psect text
global _time_string,_strcmp
_time_string:

        call    gettime
        ld      hl,_thetime
        push    hl  ; second arg for _strcmp
        ld      de,_thetime0
        push    de  ; second arg for _strcmp
        ld      bc,16
        ldir
        call    gettime
        call    _strcmp
        pop     de
        pop     de
        ld      a,l
        or      h
        call    nz,gettime
        ld      hl,_thetime
        ld      a,(hl)
        cp      '0'
        ret     nz
        inc     hl
        ret

psect lowpage
gettime:
        ld      c,35h
        ld      hl,_thetime
        call    getdigit
        call    getdigit
        inc     hl
        call    getdigit
        call    getdigit
        inc     hl
        inc     hl

        ld      a,4
        out     (4),a
        add     a,a
        out     (3),a
        ld      a,(0a057h)  ;; check format on page 408 (copy of system 402)
        rra
        jr      nc,DMYformat

        ld      c,3ah  ; month
        call    getdigit
        call    getdigit
finish:
        inc     hl
        call    getdigit
        call    getdigit
        inc     hl
        ld      c,3ch
        call    getdigit
        call    getdigit
        jp      restore_a000
DMYformat:
        ld      c,38h
        call    getdigit
        call    getdigit
        ld      c,3ah
        jr      finish

getdigit:
        in      a,(c)
        and     0fh
        add     a,'0'
        ld      (hl),a
        inc     hl
        dec     c
        ret

             ;  0123456789012345
_thetime: defm 'XX:YY  MM/DD/YY'
          defb 0
_thetime0: defs 16


psect beforebss
global $auxbuffer,$copybuffer,_iloc_and_wt
global _preparehelp,_showhelp
COND SECURE.xor.1
_memo:
global _memo
        defm 'MEMO'
        defb 0FFh,0FFh,0D0h,0Dh,00,16h,00,00,00
        defm 'YYYYMMDD'
        defb 0
        defm 'MemoTitleHere       '
        defb 0
helpdata:
*include help.inc
ENDC
COND SECURE
global __ozfile1buffer
__ozfile1buffer:
        defs 1985-1957 ;; pad a little
helpdata:
*include smain.inc
ENDC

_preparehelp:
        call    set_a000_400
        ld      hl,helpdata
        ld      de,0a968h
        ld      bc,2400
        ldir
        jp      restore_a000

_showhelp:
        call    set_a000_400
        ld      hl,0a968h
        ld      de,0a000h
        ld      bc,2400
        ldir
        jp      restore_a000

psect beforebss

global header0,header1

header0:
        defw    0 ;; page
        defw    0a400h + 12 ;; width table
        defw    0a400h ;; font delta
        defb    0 ;; height
        defb    07fh ;; mask

header1:
        defw    0 ;; page
        defw    0b180h + 12 ;; width table
        defw    0b180h ;; font delta
        defb    0 ;; height
        defb    07fh ;; mask


