psect lowpage

global _ozsetmembyte

_ozsetmembyte:
        pop     hl
        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        exx
        pop     de
        ld      c,3
        out     (c),e
        inc     c
        out     (c),d
        pop     hl
        ld      de,0a000h
        add     hl,de
        pop     de
        ld      (hl),e
        push    de
        push    hl
        push    hl
        exx
        out     (c),d
        dec     c
        out     (c),e
        jp      (hl)
