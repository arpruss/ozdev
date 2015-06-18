global _main
psect text
_main:
        ld hl,104
        push hl
        ld c,1
        ld de,0c088h+2*8
        call 7a8dh
        pop hl
        ret
