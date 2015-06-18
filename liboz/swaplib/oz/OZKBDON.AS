        psect text
; void ozkeyboardoff();
global _ozkbdoff,_ozkbdon
_ozkbdoff:
        in      a,(7)
        or      1
        out     (7),a
        ret

_ozkbdon:
        in      a,(7)
        and     0feh
        out     (7),a
        ret

