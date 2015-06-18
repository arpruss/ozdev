        psect text
global __Sozbox
; void ozbox(int x, int y, int width, int height);
__Sozbox:
        push    ix
        ld      ix,2
        add     ix,sp
        ld      l,(ix+8) ; height
        ld      h,(ix+9)
        push    hl
        ld      l,(ix+6) ; width
        ld      h,(ix+7)
        push    hl
        ld      c,(ix+4) ; y
        ld      b,(ix+5)
        ld      e,(ix+2) ; x
        ld      d,(ix+3)
        call    07f46h ; 0x617e
        pop     bc
        pop     bc
        pop     ix
        ret

