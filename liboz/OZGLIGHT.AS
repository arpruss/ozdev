global _ozgetlight,__ozbacklight
psect text
; int ozgetlight(void)
_ozgetlight:
        ld      a,(__ozbacklight)
        and     040h
        ld      l,0
        ld      h,l
        ret     z
        inc     l
        ret

