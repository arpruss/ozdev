psect text
global _ozserialin,_ozserialout,__ozserintoff,__ozserinton


__ozserintoff:
        in a,(7)
        or 4
        out (7),a
        ld a,0
        out (41h),a
        ret

__ozserinton:
        in a,(7)
        and 0ffh-4
        out (7),a
        ld a,1
        out (41h),a
        ret

_ozserialin:
        in      a,(45h)
        and     1
        jr      z,nothing
        in      a,(40h)
        ld      l,a
        ld      h,0
        ret
nothing:
        ld      hl,-1
        ret

_ozserialout:
        ld      hl,2
        add     hl,sp
waittop:
        in      a,(45h)
        and     20h
        jr      z,waittop
        ld      a,(hl)
        out     (40h),a
        ret

