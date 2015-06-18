;; miscellaneous routines

arrow3 equ 0 ; 7fh
arrow2 equ 3eh
arrow1 equ 1ch
arrow0 equ 8h

psect lowpage
upper_arrow_save:
        defs    4
lower_arrow_save:
        defs    4

global _reverselines,_reverse2bytes,_upperarrow,_lowerarrow
global _restoreupper,_restorelower,_clearbottom8

; void _upperarrow()
_upperarrow:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      bc,30
        ld      hl,0a000h+29
        ld      de,upper_arrow_save
        ld      a,(hl)
        ld      (de),a
        add     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        add     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        add     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        ld      hl,0a000h+29
        ld      (hl),arrow0
        add     hl,bc
        ld      (hl),arrow1
        add     hl,bc
        ld      (hl),arrow2
        add     hl,bc
        ld      (hl),arrow3
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

_restoreupper:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      bc,30
        ld      hl,0a000h+29
        ld      de,upper_arrow_save
        ld      a,(de)
        ld      (hl),a
        add     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        add     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        add     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

_lowerarrow:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      bc,30
        ld      hl,0a000h+2400-1
        ld      de,lower_arrow_save
        ld      a,(hl)
        ld      (de),a
        or      a
        sbc     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        sbc     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        sbc     hl,bc
        inc     de
        ld      a,(hl)
        ld      (de),a
        ld      hl,0a000h+2400-1
        ld      (hl),arrow0
        sbc     hl,bc
        ld      (hl),arrow1
        sbc     hl,bc
        ld      (hl),arrow2
        sbc     hl,bc
        ld      (hl),arrow3
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

_restorelower:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      bc,30
        ld      hl,0a000h+2400-1
        ld      de,lower_arrow_save
        ld      a,(de)
        ld      (hl),a
        or      a
        sbc     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        sbc     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        sbc     hl,bc
        inc     de
        ld      a,(de)
        ld      (hl),a
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

; void reverselines(byte y, byte lines);
;   or byte into the  at offset
_reverselines:
        push    ix
        ld      ix,2
        add     ix,sp
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      l,(ix+2) ; y
        ld      h,0
        add     hl,hl
        ld      c,l
        ld      b,h      ; bc=2*y
        add     hl,hl    ; hl=4*y
        add     hl,hl    ; hl=8*y
        add     hl,hl    ; hl=16*y
        add     hl,hl    ; hl=32*y
        sbc     hl,bc    ; hl=30*y
        ld      bc,0a000h
        add     hl,bc
        ld      c,(ix+4) ; n
        ld      a,c
        or      a
        jp      z,Done
OuterLoop:
        ld      b,29
InnerLoop:
        ld      a,(hl)
        cpl
        ld      (hl),a
        inc     hl
        djnz    InnerLoop
        ld      a,(hl)
        xor     7fh
        ld      (hl),a
        inc     hl
        dec     c
        jr      nz,OuterLoop
Done:
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret

; void clearbottom8
;;  clear bottom 8 lines
_clearbottom8:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      hl,0a000h+30*8*9
        ld      de,0a000h+30*8*9+1
        ld      (hl),0
        ld      bc,30*8-1
        ldir

        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret


; void reverse2bytes(byte y, byte lines);
;   or byte into the  at offset
_reverse2bytes:
        push    ix
        ld      ix,2
        add     ix,sp
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      l,(ix+2) ; y
        ld      h,0
        add     hl,hl
        ld      c,l
        ld      b,h      ; bc=2*y
        add     hl,hl    ; hl=4*y
        add     hl,hl    ; hl=8*y
        add     hl,hl    ; hl=16*y
        add     hl,hl    ; hl=32*y
        sbc     hl,bc    ; hl=30*y
        ld      bc,0a000h
        add     hl,bc
        ld      b,(ix+4) ; n
        ld      a,b
        or      a
        jp      z,Done2
OuterLoop2:
        ld      a,(hl)
        cpl
        ld      (hl),a
        inc     hl
        ld      a,(hl)
        cpl
        ld      (hl),a
        ld      de,29
        add     hl,de
        djnz    OuterLoop2
Done2:
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret

