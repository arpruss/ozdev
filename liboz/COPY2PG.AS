global $copytopage,_ozcopytopage

psect text
_ozcopytopage:
;; ozcopytopage(unsigned dest_page,unsigned dest_offset,char *data,unsigned length)
        exx
        pop     hl  ;; ret address
        pop     de  ;; dest_page
        exx
        ld      hl,0a000h
        pop     de
        add     hl,de
        ex      de,hl
        ;; de=destination address
        pop     hl  ;; source
        pop     bc  ;; length
        push    bc
        push    hl
        push    de
        exx
        push    de
        push    hl
        push    de
        exx
        call    $copytopage
        pop     hl
        ret

