        psect lowpage
        global __ozputsprite0,__ozactivepage
; void _ozputsprite0(unsigned offset, byte height, byte *src);
;   or byte into the  at offset
;   maximum height: 8

spritesave:
        defs    16

__ozputsprite0:
        push    ix
        push    iy
        ld      ix,4
        add     ix,sp

        ld      l,(ix+6)
        ld      h,(ix+7)
        ld      de,spritesave
        ld      a,(ix+4)
        add     a,a
        ld      c,a
        ld      b,0
        ldir

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,(__ozactivepage)
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      hl,0a000h
        ld      c,(ix+2) ; offset
        ld      b,(ix+3)
        add     hl,bc
        ld      b,(ix+4)  ; height
        ld      iy,spritesave
        ld      de,30
BlockTop:
        ld      a,(iy+1)
        cpl
        and     (hl)
        xor     (iy)
        ld      (hl),a
        inc     iy
        inc     iy
        add     hl,de
        djnz    BlockTop

        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     iy
        pop     ix
        ret

