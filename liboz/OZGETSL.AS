global __ozslotprogramname,restore_a000,_ozdetectmodel,_ozgetprogramname

psect lowpage
__ozslotprogramname:
        defs 19
        defb 0

_ozgetprogramname:
        call    _ozdetectmodel
        bit     2,l
        jr      nz,Empty

        pop     de
        pop     hl
        push    hl
        push    de

        ld      a,l
        add     a,a
        add     a,a
        add     a,60h

        out     (3),a
        xor     a
        out     (4),a

        ld      a,(0a000h)
        cp      'O'
        jr      nz,Empty
        ld      a,(0a00ah)
        inc     a
        ld      l,a
        ld      h,0a0h
        ld      bc,19
        ld      de,__ozslotprogramname
        ldir
        ld      hl,__ozslotprogramname
getout:
        jp      restore_a000
Empty:
        ld      hl,0
        jr      getout

