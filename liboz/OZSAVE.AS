global __ozclick_setting
global _ozdetectmodel,_ozsavekeysettings,_ozrestorekeysettings
global $settings
global restore_a000
global set_save_pagenum
global __ozupdate64hztime

psect lowpage

measure:
        push    ix
        ld      b,14
        ld      d,'K'
        ld      ix,'k'
MeasureTop:
        ld      e,(hl)
        add     ix,de
        inc     hl
        djnz    MeasureTop
        push    ix
        pop     hl
        pop     ix
        ret

_ozsavekeysettings:
        call    set_save_pagenum

        ld      hl,$settings
        call    measure

        ld      (0bff0h),hl
        ld      de,0bff2h
        ld      hl,$settings
        ld      bc,14
        ldir
getOut:
        jp      restore_a000

_ozrestorekeysettings:
        call    set_save_pagenum

        ld      hl,0bff2h
        call    measure

        ld      de,(0bff0h)
        or      a
        sbc     hl,de

        jr      nz,getOut

        ld      hl,0bff2h
        ld      de,$settings
        ld      bc,14
        ldir
        call    restore_a000
        jp      __ozupdate64hztime

