psect text

global ScrCharSet,__ozfontpointers,_ozsetfontpointer

header:
        defw    0407h ;; page
width_table:
        defw    0 ;; width table
font_table:
        defw    0 ;; font delta
height:
        defb    0 ;; height
mask:
        defb    07fh ;; mask

_ozsetfontpointer:
; ozsetfontpointer(byte *font);
        push    ix
        pop     de

        pop     bc
        pop     ix
        push    ix
        push    bc

        ld      a,(ix+2)
        cp      6fh
        jr      nz,getout
        ld      a,(ix+3)
        cp      46h
        jr      nz,getout

        ld      a,(ix+8)  ;; height
        ld      (height),a

        ld      a,(ix+7)  ;; last
        ld      (mask),a

        ld      (font_table),ix

        ld      bc,12
        add     ix,bc
        ld      (width_table),ix

        ld      hl,header
        ld      (__ozfontpointers+12),hl

        ld      a,6
        ld      (ScrCharSet),a

getout:
        push    de
        pop     ix
        ret
