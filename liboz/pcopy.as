global $copytopage,$copybuffer,restore_a000

psect lowpage

origpage: defw 0
copypage: defw 0

;; arguments: [stack] : page to copy to
;;                 hl : source
;;                 de : destination
;;                 bc : length
$copytopage:
        exx
        pop     hl
        pop     de
        ld      (copypage),de
        push    de
        push    hl
        exx

        ld      a,b
        or      a
        jr      z,LastPage
CopyTop:
        push    bc
        ld      bc,256
        call    copyblock
        pop     bc
        djnz    CopyTop
LastPage:
        ld      a,c
        or      a
        jp      z,restore_a000
copyblock:
        push    bc
        push    de
        ld      de,$copybuffer
        ldir
        pop     de
        pop     bc
        push    hl
        ld      a,(copypage)
        out     (3),a
        ld      a,(copypage+1)
        out     (4),a
        ld      hl,$copybuffer
        ldir
        pop     hl
        jp      restore_a000
