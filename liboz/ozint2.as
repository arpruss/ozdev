global _ozsetisr,_ozisroff

psect text
error:
        ld hl,-1
        ret
_ozsetisr:
;; load in interrupt handler stub

        ld     a,(0)
        cp     di
        jr     nz,error

        pop    bc
        pop    hl
        push   hl
        push   bc
        ld     (ozisrpointer),hl

        di

        ld     hl,ISRStub_begin
        ld     de,ISRStub_loc
        ld     bc,ISRStub_len
        ldir


        ld   a,jr
        ld   (0ffffh),a

        ld     hl,0fe00h
        ld     de,0fe01h
        ld     bc,256
        ld     a,0ffh
        ld     (hl),a
        ldir

        dec     a       ;; was:  ld a,0feh
        ld      i,a
        im      2
        ei
        ld      hl,0
        ret

ISRStub_begin:
        push    af
        push    bc
        push    de

        in      a,(1)
        ld      e,a
        in      a,(2)
        ld      d,a

        push    de

        in      a,(3)
        ld      e,a
        in      a,(4)
        ld      d,a

        push    de

        ld      a,6-4
        out     (1),a
        ld      a,4
        out     (2),a
        ld      a,7
        out     (3),a
        ld      a,4
        out     (4),a
ozisrpointer equ $+1
        call    38h

        pop     de

        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a

        pop     de

        ld      a,e
        out     (1),a
        ld      a,d
        out     (2),a

        pop     de
        pop     bc
        pop     af
        ei
        reti

;; the following code must get loaded at fff4
        di
        jp      ISRStub_loc ;; ISRStub_begin
ISRStub_end:

ISRStub_len equ ISRStub_end-ISRStub_begin
ISRStub_loc equ 0fff4h+ 3 - ISRStub_len


_ozisroff:
        im 1
        ret

