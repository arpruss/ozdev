global _ozsetcontrast,_ozgetcontrast,__ozcontrast,$ozsetlcdstate
global $ozlcdstatus

psect text
_ozgetcontrast:
        ld      a,(__ozcontrast)
        ld      l,a
        ld      h,0
        ret

_ozsetcontrast:
        pop     hl
        pop     bc
        push    bc
        push    hl
        ld      a,c
        cp      40h
        jr      c,LeaveAlone
        ld      a,3fh
LeaveAlone:
        ld      (__ozcontrast),a
        ld      e,a
        ld      bc,($ozlcdstatus)
        ld      a,c
        and     0ffh-3fh
        or      e
        ld      c,a
        push    bc
        call    $ozsetlcdstate
        pop     bc
        ret

