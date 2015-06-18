global __ozkeyrepeatspeed,__ozkeyrepeatdelay
global _ozgetrepeatspeed,_ozgetrepeatdelay,_ozsetrepeatspeed,_ozgetrepeatspeed
global _ozsetrepeatdelay
global __ozupdate64hztime

psect text

_ozgetrepeatspeed:
        ld      a,(__ozkeyrepeatspeed)
getout:
        ld      l,a
        ld      h,0
        ret

_ozgetrepeatdelay:
        ld      a,(__ozkeyrepeatdelay)
        jr      getout

_ozsetrepeatspeed:
        pop     hl
        pop     bc
        push    bc
        push    hl
        ld      a,c
        ld      (__ozkeyrepeatspeed),a
        jp      __ozupdate64hztime

_ozsetrepeatdelay:
        pop     hl
        pop     bc
        push    bc
        push    hl
        ld      a,c
        ld      (__ozkeyrepeatdelay),a
        jp      __ozupdate64hztime

