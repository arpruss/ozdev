psect text
global _ozfindmemo,_ozopenmemo,_ozopenfile
global page,offset,recnum,ozfr$ret_error

_ozopenmemo:
;; void ozopenmemo(char *s);
        pop     hl
        pop     de
        push    de
        push    hl
        ld      hl,0
        ld      (page),hl
        ld      (offset),hl
        ld      bc,recnum
        push    bc
        ld      bc,offset
        push    bc
        ld      bc,page
        push    bc

        push    de

        call    _ozfindmemo

        pop     de
        pop     de
        pop     de
        pop     de

        ld      a,l
        or      h

        jp      z,ozfr$ret_error

        push    hl
        call    _ozopenfile
        pop     de

        ret

psect bss
page: defs 2
offset: defs 2
recnum: defs 2

