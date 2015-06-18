        psect lowpage
        global _ozcls

; void ozcls(void)
_ozcls:
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

        ld      de,0a000h
        ld      bc,2400
        ld      l,0
        call    7f12h

       pop     bc

       ld      a,c
       out     (3),a
       ld      a,b
       out     (4),a
       ret
