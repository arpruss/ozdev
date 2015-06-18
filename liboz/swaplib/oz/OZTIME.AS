        psect   text
        global  _ozsec1,_ozsec2,_ozmin1,_ozmin2
; int ozsec1();
;   returns first digit of time in seconds
_ozsec1:
        in a,(30h)
        and     7
        ld      l,a
        ld      h,0
        ret

; int ozsec2();
;   returns second digit of time in seconds
_ozsec2:
        in a,(31h)
        and     7
        ld      l,a
        ld      h,0
        ret

; int ozmin1();
;   returns first digit of time in minutes
_ozmin1:
        in      a,(32h)
        and     7
        ld      l,a
        ld      h,0
        ret

; int ozmin2();
;   returns second digit of time in minutes
_ozmin2:
        in      a,(33h)
        and     7
        ld      l,a
        ld      h,0
        ret

