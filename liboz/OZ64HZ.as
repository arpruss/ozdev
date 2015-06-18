psect text

global __ozi64hztime,_oz64hztimeron,$ozupdate64hztime
global _ozget64hztimerstate,__ozi64hzcount
global KbdHit

_ozget64hztimerstate:
    ld  a,(__ozi64hztime)
    ld  l,0
    or  a
    ret nz
    inc l
    ret

_oz64hztimeron:
    in  a,(7)
    and 255-32  ;; turn on 64hz interrupt
    or  1       ;; and turn off keyboard interrupt
    out (7),a
    xor a
    ld  (__ozi64hztime),a
    ld  (__ozi64hzcount),a
    ret


