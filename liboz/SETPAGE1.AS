psect lowpage
global set_a000_400
set_a000_400:
        xor     a
        out     (3),a
        ld      a,4
        out     (4),a
        ret

