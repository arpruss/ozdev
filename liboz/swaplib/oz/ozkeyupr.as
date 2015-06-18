        psect   text
        global _ozkeyupper,_ozkeylower,$gint


; int ozkeyupper(char mask);
;   returns key pressed bitmap for upper part of keyboard
_ozkeyupper:
        ld      hl,2
        add     hl,sp
        call    $gint
        push    hl     ; pushed byte
        pop     bc
        xor     a
        out     (17),a
        ld      a,c
        out     (18),a
        in      a,(16)
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

; int ozkeylower(char mask);
;   returns key pressed bitmap for lower part of keyboard
_ozkeylower:
        ld      hl,2
        add     hl,sp
        call    $gint
        push    hl     ; pushed byte
        pop     bc
        xor     a
        out     (18),a
        ld      a,c
        out     (17),a
        in      a,(16)
        ld      c,a
        ld      b,0
        push    bc
        pop     hl
        ret

