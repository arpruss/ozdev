global _test1
psect text
_test1:
        xor a
        or a
        xor 0ffh
        jr nz,OK
        ld hl,0
        ret
OK:
        ld hl,1
        ret
