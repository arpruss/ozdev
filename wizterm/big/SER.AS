psect text
global _ozserialout_echo, _echo, _termputch

_ozserialout_echo:
        ld      hl,2
        add     hl,sp
waittop:
        in      a,(45h)
        and     20h
        jr      z,waittop
        ld      a,(hl)
        out     (40h),a
        ld      l,a
        ld      a,(_echo)
        or      a
        ret     z
        ld      h,0
        push    hl
        call    _termputch
        pop     hl
        ret

psect bss
_echo:  defs 1
