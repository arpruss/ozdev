
global set_save_pagenum,_ozwritecustomfontmem,_ozsave_pagenum
global $copytopage, restore_a000
psect lowpage

_ozwritecustomfontmem: ;; (byte fnum,byte *data)
        call    set_save_pagenum
        call    restore_a000

        exx
        pop     bc
        exx
        pop     de  ;; fnum
        pop     hl  ;; data

        inc     hl
        inc     hl
        inc     hl
        inc     hl

        ld      c,(hl) ;; length of font
        inc     hl
        ld      b,(hl)

        dec     hl
        dec     hl
        dec     hl
        dec     hl
        dec     hl

;        pop     bc  ;; length
;        push    bc
        push    hl
        push    de
        exx
        push    bc
        exx


        ld      a,e

        ld      de,(_ozsave_pagenum)
        push    de

        or      a
        ld      de,0a400h
        jr      z,Font0
        ld      de,0b180h
Font0:
        call    $copytopage

        pop     de
        ret
