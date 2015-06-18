global _ozsavescreen,_ozrestorescreen,$ozcopy,__ozactivepage

psect text
_ozsavescreen:
        ld      de,0
        push    de
        ld      hl,968h
        push    hl
        ld      hl,(__ozactivepage)
        ld      d,4  ;; e=0 still
copy:
        ld      bc,2400
        call    $ozcopy
        pop     hl
        pop     hl
        ret

_ozrestorescreen:
        ld      de,968h
        push    de
        ld      hl,0
        push    hl
        ld      h,4  ;; l=0 still
        ld      de,(__ozactivepage)
        jr      copy

