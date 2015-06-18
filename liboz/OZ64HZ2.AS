psect text
global __ozkeyrepeatspeed,__ozkeyrepeatdelay,__ozi64hztime
global __ozupdate64hztime,$ozupdate64hztime,KbdHit
global _oz64hztimeroff

__ozupdate64hztime:
    ld  a,(__ozi64hztime)
    or  a
    ret z
_oz64hztimeroff:
$ozupdate64hztime:
    ld  a,(__ozkeyrepeatspeed)
    ld  c,a
    ld  a,(__ozkeyrepeatdelay)
    cp  c
    jr  c,noSwap
    ld  a,c
noSwap:
    inc a
    ld  (__ozi64hztime),a
    jp  KbdHit
