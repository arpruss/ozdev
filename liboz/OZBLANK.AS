;; LCDStatus is 0c024h in the OS

global _ozunblankscreen,_ozslow,$blanked,$ozsetlcdstate
global _ozblankscreen,_ozfast,$ozsetlcdstate,$ozlcdstatus
global $init_unblank

psect text
_ozblankscreen:
        ld      hl,_ozslow
        ld      ($init_unblank),hl

        ld      a,1
        ld      ($blanked),a
        ld      hl,($ozlcdstatus)
        ld      a,l
        or      40h
        and     7fh
        ld      l,a
        push    hl
        call    $ozsetlcdstate
        pop     hl
        ret

_ozfast:
        call    _ozblankscreen
        xor     a
        out     (20h),a
        out     (24h),a
        ret


psect lowpage  ;; needs to be accessible to init code
$blanked:
        defb    0

psect text
_ozslow:
_ozunblankscreen:
        ld      hl,$blanked
        ld      a,(hl)
        or      a
        ret     z
        inc     a
        ld      (hl),a
        ld      a,5
        out     (24h),a
        ld      a,1
        out     (20h),a
        ld      hl,($ozlcdstatus)
        ld      a,l
        and     0bfh
        or      80h
        ld      l,a
        push    hl
        call    $ozsetlcdstate
        pop     hl
        ret


