psect text
global _fabs
_fabs:
        pop     bc
        pop     de
        pop     hl
        push    hl
        push    de
        push    bc

        ld      a,h
        and     7fh
        ld      h,a
        ret
