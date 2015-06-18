global fs_init,$cdir_pages,_ozinitfs,__ozmodel,_ozfindrecnum,restore_a000

psect lowpage

search_for:
        defw 0

_ozfindrecnum:
        ld      a,(fs_init)
        or      a
        call    z,_ozinitfs

        pop     hl
        pop     de  ;; search_for
        push    de
        push    hl

        ld      (search_for),de

        dec     de

        ld      l,e
        ld      h,d
        add     hl,hl   ;; should not have carry, I expect

        ld      a,h     ;; a=hl/256
        srl     a       ;; a=hl/512
        srl     a       ;; a=hl/1024
        srl     a       ;; a=hl/2048
        srl     a       ;; a=hl/4096
        srl     a       ;; a=hl/8192

        add     a,0ch
        ld      c,a
        ld      b,4

        cp      010h
        jr      c,not_500

        and     0fh
        ld      c,a
        inc     b
not_500:

        ld      a,h
        and     1fh
        add     a,0a0h
        ld      h,a

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      c,(hl)
        inc     hl
        ld      a,(hl)     ;; ac=AddressOfLocation

        ld      hl,__ozmodel
        bit     1,(hl)
        jr      z,Not770

        sub     080h
        jp      c,PageOut_error
        ld      b,a
        or      c
        jp      z,PageOut_error
        jr      SkipNot770

Not770:
        cp      080h
        jp      nc,PageOut_error
        ld      b,a
SkipNot770:
;;
;; now, bc=location in directory, hl=__ozmodel
;;
        ld      a,b
        srl     a
        bit     1,(hl)
        jr      nz,Is770
        srl     a
Is770:
;; Now, a=page in dir
        ld      e,a
        srl     a
        srl     a
        srl     a
;; Now, a=block in dir
        push    bc
        ld      bc,$cdir_pages
        add     a,a  ;; block*2
        ld      l,a
        ld      h,0
        add     hl,bc
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        ld      a,e
        and     7
        ld      e,a
        ld      d,0
        add     hl,de
;; hl=cdir page number
;; page in right cdir page number

        ld      a,l
        out     (3),a
        ld      a,h
        out     (4),a

;; bc=location*8 (or *16)
        pop     bc
        sll     c
        rl      b
        sll     c
        rl      b
        sll     c
        rl      b
        ld      a,(__ozmodel)
        and     2
        jr      z,Not770_2
        sll     c
        rl      b
Not770_2:

        ld      a,b
        and     1fh
        ld      b,a
        ld      hl,0a000h
        add     hl,bc


        ld      de,(search_for)
        ld      a,(hl)
        cp      e
        jr      nz,PageOut_error
        inc     hl
        ld      a,(hl)
        cp      d
        jr      nz,PageOut_error
        ld      bc,7
        ld      a,(__ozmodel)
        and     2
        jr      z,Not770_3
        ld      c,15
Not770_3:
        add     hl,bc
SearchForLoc:
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)
        ld      a,e
        and     d
        inc     a
        jr      z,SearchForLoc  ; FFFF

        ex      de,hl
        jp      restore_a000

PageOut_error:
        ld      hl,0
        jp      restore_a000
