psect text
global _ozopenfile,__ozloc
global __ozposinbuffer
global _ozloadcluster,_ozinclocation,_ozreadfilebyte,_ozclosefile
global __ozfilebuffer,ozfr$ret_error

load_in:
        push    hl
        call    _ozloadcluster
        ld      de,__ozfilebuffer
        ld      bc,64
        ldir
        pop     hl
        ret

_ozopenfile:
;; int ozopenfile(unsigned loc);
        pop     hl
        pop     de
        ld      (__ozloc),de
        push    de
        push    hl

        push    de
        ex      de,hl
        call    load_in
        pop     de

        ld      a,2
        ld      (__ozposinbuffer),a

        ld      hl,0
        ret

_ozreadfilebyte:
;; int ozreadfilebyte(void)
        ld      hl,(__ozloc)
        ld      a,l
        or      h
        jr      z,ret_error

        ld      hl,__ozfilebuffer
        ld      a,(__ozposinbuffer)
        ld      e,a
        ld      d,0
        add     hl,de
        ld      l,(hl)    ;; output byte
        ld      h,d       ;; h=0
        push    hl

        inc     e
        ld      a,e
        cp      64
        jr      nc,IncCluster
        ld      (__ozposinbuffer),a
        pop     hl
        ret

IncCluster:
        ld      hl,(__ozfilebuffer)

        ld      a,l
        and     h
        inc     a
        jr      z,Final   ;; header = -1 --> end of file

        ld      hl,(__ozloc)
        push    hl
        call    _ozinclocation
        pop     de

        ld      (__ozloc),hl
        ld      a,2
        ld      (__ozposinbuffer),a

        call    load_in

        pop     hl
        ret

Final:
        inc     hl  ;; 0->hl
        ld      (__ozloc),hl

        pop     hl
        ret

ozfr$ret_error:
ret_error:
        ld      hl,-1
        ret

_ozclosefile:
        ld      hl,0
        ld      (__ozloc),hl
        ret


psect bss
__ozloc:    defs 2
__ozposinbuffer: defs 1
__ozfilebuffer: defs 64
