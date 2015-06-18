psect text
global _ozgetchblank,__oz1hz,__ozautoblanktime,__ozautoofftime,__ozi64hztime
global __ozautolightofftime
global _oz64hztimeroff,_ozexitto,_ozsetlight,_ozgetch,_ozfast,_ozslow
global _oz64hztimeron,__ozgetch
global KeyBufGetPos
global __ozprogoptions
global __ozgetchhook,__ozdisablepowerkey

_ozgetch:
_ozgetchblank:
        xor     a
        ld      (blanked),a
        ld      (lightoff),a
        ld      a,(__ozdisablepowerkey)
        ld      (powerstate),a

        ld      de,(__oz1hz)  ;; start timer

        ld      a,(__ozi64hztime)
        ld      (timerstate),a

Top:
        di
        ld      hl,KeyBufGetPos
        ld      a,(hl)
        inc     hl
        cp      (hl)
        jp      nz,CleanUp
        ei
        halt

        ld      hl,(__oz1hz)
        or      a
        sbc     hl,de

__ozgetchhook:
        push    de
        push    hl
        push    hl
        nop
        nop
        nop     ;; these instructions may get replaced by a call
                ;; to a routine that takes one unsigned argument
                ;; which is the time since start of count
        pop     hl
        pop     hl
        pop     de

        push    hl

        ld      bc,(__ozautoofftime)
        ld      a,c
        or      b

        jr      z,SkipOffCheck

        sbc     hl,bc
        jr      c,SkipOffCheck

        ld      hl,803ah    ;; power key
        push    hl
        call    _ozexitto

SkipOffCheck:
        pop     hl
        push    hl

        ld      a,(lightoff)
        or      a
        jr      nz,SkipLightCheck

        ld      bc,(__ozautolightofftime)
        ld      a,c
        or      b
        jr      z,SkipLightCheck

        sbc     hl,bc
        jr      c,SkipLightCheck

        push    de

        ld      l,0
        push    hl
        call    _ozsetlight
        pop     hl

        pop     de

SkipLightCheck:
        pop     hl

        ld      a,(blanked)
        or      a
        jr      nz,Top
        ld      bc,(__ozautoblanktime)
        ld      a,c
        or      b
        jr      z,Top

        sbc     hl,bc
        jr      c,Top

        push    de

        call    _oz64hztimeroff
        call    _ozfast
        xor     a
        out     (8),a   ; power down
        inc     a
        ld      (blanked),a
        ld      (__ozdisablepowerkey),a

        pop     de

        jr      Top

CleanUp:
        ei

        ld      a,(blanked)
        or      a
        jr      z,NotBlanked

;;        ld      a,1 (a=1 if blanked)
        out     (8),a
        call    _ozslow
        ld      a,(powerstate)
        ld      (__ozdisablepowerkey),a
        ld      a,(timerstate)
        or      a
        call    z,_oz64hztimeron
        ld      a,(__ozprogoptions)
        and     4
        jr      nz,WaitForKey
NotBlanked:
        jp      __ozgetch
WaitForKey:
        call    __ozgetch
        jp      _ozgetchblank

psect bss
timerstate: defs 1
blanked: defs 1
lightoff: defs 1
powerstate: defs 1
__ozgetchtimerstart: defs 2
