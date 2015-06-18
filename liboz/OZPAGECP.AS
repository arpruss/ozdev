global _ozcopypage,$ozcopy

psect text
_ozcopypage:
;; void ozcopypage(byte dest, byte src)
        pop     bc
        pop     de
        pop     hl
        push    hl
        push    de
        push    bc

        ld      a,l  ; source page
        cp      e    ; dest page
        ret     z    ; if z, copy page to same page -- exit

        or      a

        ld      de,0
        push    de
        push    de   ; copy offsets = 0 for both pages
        ld      bc,2400 ;; screen size
        ld      h,4

        jr      z,Copy01

;; Copy page 1 to page 0
        ld      l,h  ; hl=404h
        ld      d,l  ; de=400h
        call    $ozcopy
        pop     de
        pop     de
        ret
Copy01:  ;; Copy page 0 to page 1
        ld      l,e  ; hl=400h
        ld      e,h
        ld      d,h  ; de=404h
        call    $ozcopy
        pop     de
        pop     de
        ret

