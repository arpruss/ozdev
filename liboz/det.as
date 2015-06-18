psect lowpage
global $is750

$is750:
;;
;; zero flag = not 750
;; not zero flag = 750
;;
        call    07f94h
        or      a
        ret     z  ;; 700/730

;; at this point we have either a 770 or a 750PC or a 750M

        ld      a,0ch
        ld      hl,0ab4fh
        ld      de,String1
        call    compare4
        ret     nz   ;; is neither 770 nor 7xxM, hence is 750PC
;; we have either a 770 or a 750M at this point
        ld      a,0eh  ;; we'll check where the font tables are
        ld      hl,0a300h
        ld      de,String2
;;        jp      compare4   ;; z if 770;  nz if 750M

compare4:
        out     (3),a
        xor     a
        out     (4),a
        ld      b,4

top:
        ld      a,(de)
        cp      (hl)
        jr      nz,getoutofcompare
        inc     de
        inc     hl
        djnz    top

getoutofcompare:
;; change if needed
        ld      a,7
        out     (3),a
        ld      a,4
        out     (4),a

        ret

String1:
        defb    89h,0c0h,19h,4dh
String2:
        defm    'PC_P'

global _main,_printf,_ozgetch

_main:
        call    $is750
        ld      hl,no_string
        jp      z,IsNot
        ld      hl,yes_string
IsNot:
        push    hl
        call    _printf
        pop     hl
        jp      _ozgetch
no_string:
        defm 'Not a 750'
        defb 0
yes_string:
        defm 'Is a 750'
        defb 0
