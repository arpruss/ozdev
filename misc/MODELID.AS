global _ozgetmodel

psect text
_ozgetmodel:
        call 7f94h
        ld l,a
        ld h,0
        ret
