psect text
global _ozparity,_ozstopbits,_ozgetlcr,_ozsetlcr,_ozsetbaud,_ozgetbaud
global _ozdatabits

_ozdatabits:
        ld      hl,2
        add     hl,sp
        in      a,(43h)
        and     0FFh-3
        or      (hl)
        out     (43h),a
        ret

_ozsetlcr:
        ld hl,2
        add hl,sp
        ld a,(hl)
        out (43h),a
        ret

_ozparity:
        ld hl,2
        add hl,sp
        in a,(43h)
        and 1+2+4+40h+80h
        or (hl)
        out (43h),a
        ret

_ozstopbits:
        ld hl,2
        add hl,sp
        in a,(43h)
        and 1+2+8+10h+20h+40h+80
        or (hl)
        out (43h),a
        ret

_ozgetlcr:
        in a,(43h)
        ld l,a
        ld h,0
        ret

_ozsetbaud:
        ld hl,2
        add hl,sp
        in a,(43h)
        ld b,a
        or 80h
        out (43h),a
        ld a,(hl)
        out (40h),a
        inc hl
        ld a,(hl)
        out (41h),a
        ld a,b
        and 7fh
        out (43h),a
        ret

_ozgetbaud:
        in a,(43h)
        ld b,a
        or 80h
        out (43h),a
        in a,(40h)
        ld l,a
        in a,(41h)
        ld h,a
        ld a,b
        and 7fh
        out (43h),a
        ret

