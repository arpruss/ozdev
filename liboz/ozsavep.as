
global set_save_pagenum,_ozsave_pagenum,_ozdetectmodel
psect lowpage
_ozsave_pagenum: defw 0

set_save_pagenum:
        ld      bc,(_ozsave_pagenum)
        ld      a,c
        or      b
        jr      nz,SetIt

        call    _ozdetectmodel
        ld      a,l
        xor     1     ;; now, bit 0 is zero if 750/770
        and     1+2   ;; this is 0 if 750/770 & <770, i.e., if 750
        ld      bc,40fh ;; if not 750
        jr      nz,SetIt
        ld      bc,503h
SetIt:
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ld      (_ozsave_pagenum),bc
        ret

