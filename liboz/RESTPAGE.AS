psect lowpage
global restore_a000
restore_a000:
        ld      a,7
        out     (3),a
        ld      a,4
        out     (4),a
        ret


