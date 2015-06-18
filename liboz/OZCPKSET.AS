global _ozcopytocursettings,_ozcopyfromcursettings
global __ozupdate64hztime
global $settings

psect text

_ozcopytocursettings:
        pop     bc
        pop     hl      ; dest
        push    hl
        push    bc
        ld      de,$settings
        call    DoCopy
        jp      __ozupdate64hztime

_ozcopyfromcursettings:
        pop     bc
        pop     de
        push    de
        push    bc
        ld      hl,$settings
DoCopy:
        ld      bc,14
        ldir
        ret
