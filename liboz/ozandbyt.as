        psect lowpage

        global _ozdisplayandbyte,__ozactivepage
; void ozdisplayandbyte(int offset, char byte);
;   put byte into the display at offset
_ozdisplayandbyte:
        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        ld      hl,(__ozactivepage)
        out     (c),h
        dec     c
        out     (c),l

        pop     hl  ;; return address

        exx
        pop     hl  ;; offset
        pop     bc  ;; value
        ld      a,h
        add     a,0a0h
        ld      h,a
        ld      a,(hl)
        and     c
        ld      (hl),a
        push    bc
        push    hl
        exx

        out     (c),e
        inc     c
        out     (c),d

        jp      (hl)

