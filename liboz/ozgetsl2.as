global _ozgetslotnumber,__ozslotprogramname,_ozrunslot,_exit,_ozgetprogramname
global _ozungetch,restore_a000,_ozkeyclear,_ozdetectmodel

psect text
_ozgetslotnumber:
        pop     hl
        pop     de
        push    de
        ld      (location),de
        push    hl

        ld      hl,0
top:
        ld      a,l
        cp      10
        jr      nc,NotFound
        push    hl

        push    hl
        call    _ozgetprogramname
        pop     de
        ld      a,l
        or      h
        jr      z,loop_continue
        ld      de,(location)
        call    7f15h ; strcmp de,hl
        ld      a,l
        or      h
        jr      nz,loop_continue

        pop     hl
        ret

loop_continue:
        pop     hl
        inc     l
        jr      top

NotFound:
        ld      hl,-1
        ret

ungetch2:
        push    hl
        push    hl
        call    _ozungetch
        pop     hl
        call    _ozungetch
        pop     hl
        ret

psect lowpage
_ozrunslot:

        call    _ozkeyclear
        ld      hl,7015h  ; MY PROGRAMS
        call    ungetch2

        call    _ozdetectmodel
        bit     2,l
        jp      nz,_exit

        pop     de
        pop     hl      ; slot number
        push    hl
        push    de


        xor     a
        out     (4),a

        ld      c,60h
        inc     l
        ld      de,0
top1:
        ld      a,c
        out     (3),a
        add     a,4
        ld      c,a
        ld      a,(0a000h)
        cp      'O'
        jr      nz,NotProg
        inc     de
NotProg:
        dec     l
        jr      nz,top1
        dec     de

        call    restore_a000
        ld      hl,'1'
        add     hl,de
        call    ungetch2
        jp      _exit

psect bss
location: defs 2
