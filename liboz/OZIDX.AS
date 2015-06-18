global _ozgetappindexlist,__ozmodel,_ozdetectmodel
global $filetypetable,Model32k,restore_a000

psect lowpage

; void ozgetappindexlist(byte typenum, app_index_list_t *ilist);

_ozgetappindexlist:
        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel

        ld      de,0402h

        ld      a,Model32k
        or      a
        jr      z,model16
        ld      e,8   ;; de=408h
model16:
        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a

        pop     hl
        exx

        pop     hl  ;; L=typenum

        ld      h,0
        add     hl,hl
        add     hl,hl
        add     hl,hl  ;; hl=typenum*8
        ld      bc,($filetypetable)
        add     hl,bc  ;; index list
        ld      bc,-2000h
        add     hl,bc  ;; adjust to loading in paged page

        ld      de,data
        ld      bc,8
        ldir

        pop     de
        ld      ($output),de
        push    de
        push    de

        exx

        push    hl

        call    restore_a000

        ld      hl,data
        ld      de,0
$output equ $-2
        ld      bc,8
        ldir

        ret

data:
        defw 0,0,0,0

