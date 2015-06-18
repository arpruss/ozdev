psect text
global _ozserialin,_ozserialout,_ozserintoff,_ozserinton


_ozserintoff:
        in a,(7)
        or 4
        out (7),a
        ret

_ozserinton:
        in a,(7)
        and 0ffh-4
        out (7),a
        ret

_ozserialin:
        in a,(45h)
        and 1
        jr z,nothing
        in a,(40h)
        ld l,a
        ld h,0
        ret
nothing:
        ld hl,-1
        ret

_ozserialout:
        in      a,(45h)
        and     20h
        jr      z,_ozserialout
        ld      hl,2
        add     hl,sp
        ld      a,(hl)
        out     (40h),a
        ret

