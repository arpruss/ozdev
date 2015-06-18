global _ozsetautorun,_ozgetautorun
psect lowpage

_ozgetautorun:
        in      a,(4)
        push    af
        in      a,(3)
        push    af
        xor     a
        out     (3),a
        ld      c,4
        out     (c),c

        ld      a,(0bf2ch)
        ld      l,a
        ld      h,0

        pop     af
        out     (3),a
        pop     af
        out     (4),a
_ozsetautorun:
        ret

