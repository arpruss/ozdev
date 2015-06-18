        psect lowpage
        global _ozandblock,_ozorblock
; void ozandblock(int offset, char newbyte, byte num);
;   or byte into the  at offset
_ozandblock:
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

        ld      hl,0a000h
        ld      c,(ix+2) ; offset
        ld      b,(ix+3)
        add     hl,bc
        ld      de,30
        ld      b,(ix+6)  ; number
        ld      c,(ix+4)  ; newbyte
BlockTop:
        ld      a,(hl)
        and     c   ; byte
        ld      (hl),a
        add     hl,de
        djnz    BlockTop
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret

_ozorblock:
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

        ld      hl,0a000h
        ld      c,(ix+2) ; offset
        ld      b,(ix+3)
        add     hl,bc
        ld      de,30
        ld      b,(ix+6)
        ld      c,(ix+4)
BlockTop2:
        ld      a,(hl)
        or      c   ; byte
        ld      (hl),a
        add     hl,de
        djnz    BlockTop2
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret


