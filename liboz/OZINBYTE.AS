        psect   lowpage
        global  _ozdisplayinbyte,__ozactivepage
; int ozdisplayinbyte(int offset);
;   get byte from the display at offset
_ozdisplayinbyte:
        push    ix
        ld      ix,2
        add     ix,sp
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

        ld      l,(ix+2) ; offset
        ld      h,(ix+3)
        ld      bc,0a000h
        add     hl,bc
        ld      a,(hl)
        ld      l,a
        xor     a
        ld      h,a

        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret

