        psect lowpage
        global _ozdisplayandbyte
; void ozdisplayandbyte(int offset, char byte);
;   or byte into the display at offset
_ozdisplayandbyte:
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

        ld      l,(ix+2) ; offset
        ld      h,(ix+3)
        ld      bc,0a000h
        add     hl,bc
        ld      a,(hl)
        and     (ix+4)   ; byte
        ld      (hl),a

        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret

