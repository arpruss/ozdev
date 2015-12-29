global _ozsetautorun,_ozgetautorun
psect lowpage

_ozsetautorun:
        pop     hl
        pop     de
        push    de
        in      a,(4)
        push    af
        in      a,(3)
        push    af
        xor     a
        out     (3),a
        ld      c,4
        out     (c),c

        ld      a,e
        ld      (0bf2ch),a

        pop     af
        out     (3),a
        pop     af
        out     (4),a
        jp      (hl)

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
        ret

