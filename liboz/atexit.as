global _atexit,__ozcallexitfunctions

psect text
_atexit:
        ld      a,3*10
last equ $-1
        or      a
        jr      z,error
        dec     a
        dec     a
        dec     a
        ld      (last),a
Empty:
        pop     bc
        pop     de
        push    de
        push    bc
        ld      hl,__ozcallexitfunctions
        ld      c,a
        ld      b,0
        add     hl,bc
        di
        ld      (hl),call
        inc     hl
        ld      (hl),e
        inc     hl
        ld      (hl),d
        ei
        ld      hl,0
        ret
error:
        ld      hl,-1
        ret

