
global set_save_pagenum,_ozwritecustomfontbyte,restore_a000
psect lowpage

_ozwritecustomfontbyte: ;; (byte fnum,unsigned offset,unsigned value)
        call    set_save_pagenum

        exx
        pop     bc
        exx
        pop     de  ;; fnum
        pop     hl
        pop     bc  ;; value
        push    bc
        push    hl
        push    de
        exx
        push    bc
        exx

        ld      a,e
        or      a
        ld      de,0a400h
        jr      z,Font0
        ld      de,0b180h
Font0:
        add     hl,de
        ld      a,h
        cp      0bfh
        jr      nc,getout
        cp      0a0h
        jr      c,getout
        ld      (hl),c
getout:
        jp      restore_a000


