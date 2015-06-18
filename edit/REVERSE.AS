global _reverse

psect lowpage
_reverse:
;; reverse(start_address,length)
;;   reverses order of bytes in a memory area
;;   clobbers: de,hl,bc,af
        pop     de
        pop     hl
        pop     bc
        push    bc
        push    hl
        push    de

        dec     bc

        ld      e,l
        ld      d,h

        add     hl,bc

LoopTop:
        or      a
        sbc     hl,de
        ret     nc
        add     hl,de   ;; undo subtraction

        ld      c,(hl)
        ld      a,(de)
        ld      b,a
        ld      a,c
        ld      (de),a  ;; (original hl)->(de)
        ld      (hl),b  ;; (original de)->(hl)
        dec     hl
        inc     de
        jp      LoopTop
        ret
