psect text
global _ozportin, _ozportout

; char ozportin(char port)
_ozportin:
        ld      hl,2
        add     hl,sp
        ld      c,(hl)
        in      l,(c)
        ld      h,0
        ret

; void ozportout(char port, char value)
_ozportout:
        push    ix
        ld      ix,4
        add     ix,sp
        ld      c,(ix)
        ld      a,(ix+2)
        out     (c),a
        pop     ix
        ret

