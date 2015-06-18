global set_save_pagenum,restore_a000,__ozfontpointers
global _ozclearcustomfontslot
psect lowpage
_ozclearcustomfontslot:
        call    set_save_pagenum
        pop     bc
        pop     hl
        push    hl
        push    bc
        ld      a,l
        ld      hl,0b182h
        or      a
        jr      nz,UpperSlot
        ld      hl,0a400h
UpperSlot:
        ld      (hl),0
        inc     hl
        ld      (hl),0
        jp      restore_a000
