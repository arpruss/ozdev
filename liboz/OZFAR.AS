global $ozcopy,_ozfarmemcpy

psect   text

_ozfarmemcpy:
;; ozfarmemcpy(unsigned dest_page,unsigned dest_offset,unsigned src_page,
;;   unsigned src_offset,unsigned length)
        push    ix
        ld      ix,4
        add     ix,sp
        ld      e,(ix+6)
        ld      d,(ix+7)
        push    de
        ld      e,(ix+2)
        ld      d,(ix+3)
        push    de
        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      e,(ix+0)
        ld      d,(ix+1)
        ld      c,(ix+8)
        ld      b,(ix+9)
        call    $ozcopy
        pop     de
        pop     de
        pop     ix
        ret

