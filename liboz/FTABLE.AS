global LowerFontPage2,LowerFontPage1,HigherFontPage1,HigherFontPage2
global __ozfontpointers

psect lowpage
__ozfontpointers:
        defw    Font0
        defw    Font1
        defw    Font2
        defw    Font3
        defw    Font1
        defw    Font1
        defw    Font1
        defw    Font1

Font0:  ;; PC_LARGE
        defb    0eh
LowerFontPage2  equ $-1
        defb    0
        defw    0ab10h
        defw    0ab10h+310h
        defb    13
        defb    0ffh

Font1:  ;; PC_PLANE:
        defb    0eh
LowerFontPage1  equ $-1
        defb    0
        defw    0a000h
        defw    0a000h+310h
        defb    8
        defb    0ffh

Font2:   ;; EO7LARGE
        defb    0fh
HigherFontPage2 equ $-1
        defb    0
        defw    0ab10h
        defw    0ab10h+310h
        defb    13
        defb    0ffh

Font3:   ;; EO7PLANE:
        defb    0fh
HigherFontPage1 equ $-1
        defb    0
        defw    0a000h
        defw    0a000h+310h
        defb    8
        defb    0ffh

