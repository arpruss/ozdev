        global  $gint
        psect   text
; int ozsec1();
;   returns first digit of time in seconds
_ozsec1:
        in a,(30h)
        and     7
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozsec2();
;   returns second digit of time in seconds
_ozsec2:
        in a,(31h)
        and     7
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozmin1();
;   returns first digit of time in minutes
_ozmin1:
        in      a,(32h)
        and     7
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozmin2();
;   returns second digit of time in minutes
_ozmin2:
        in      a,(33h)
        and     7
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozkey();
;   returns key pressed
_ozkey:
        ld      hl,98b0h
        ld      a,0
        call    4ah
        ret

; int ozkeyupr(char mask);
;   returns key pressed bitmap for upper part of keyboard
_ozkeyupr:
        ld      hl,2
        add     hl,sp
        call    $gint
        push    hl     ; pushed byte
        pop     bc
        xor     a
        out     (17),a
        ld      a,c
        out     (18),a
        in      a,(16)
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozkeylwr(char mask);
;   returns key pressed bitmap for lower part of keyboard
_ozkeylwr:
        ld      hl,2
        add     hl,sp
        call    $gint
        push    hl     ; pushed byte
        pop     bc
        xor     a
        out     (18),a
        ld      a,c
        out     (17),a
        in      a,(16)
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

        global  _ozkey
        global  _ozkeylwr
        global  _ozkeyupr
        global  _ozsec1
        global  _ozsec2
        global  _ozmin1
        global  _ozmin2

