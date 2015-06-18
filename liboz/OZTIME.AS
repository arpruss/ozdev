        psect   text
        global  _ozsec,_ozmin,_ozmonth,_ozyear,_ozday,_ozhour,_ozweekday

; unsigned ozsec()
_ozsec:
        ld      c,31h
Compute:
        in      a,(c)
        and     0fh
        add     a,a
        ld      b,a
        add     a,a
        add     a,a
        add     a,b
        ld      b,a      ; b=10*(c)
        dec     c
        in      a,(c)
        and     0fh
        add     a,b
GetOut2:
        ld      l,a
        ld      h,0
        ret

; unsigned ozmin()
_ozmin:
        ld      c,33h
        jr      Compute

; unsigned ozhour()
_ozhour:
        ld      c,35h
        jr      Compute

; unsigned ozweekday()
_ozweekday:
        in      a,(36h)
        and     0fh
        jr      GetOut2

; unsigned ozday()
_ozday:
        ld      c,38h
        jr      Compute

; unsigned ozmonth()
_ozmonth:
        ld      c,3ah
        jr      Compute

; unsigned ozyear()
_ozyear:
        ld      c,3ch
        call    Compute
        ld      de,2000
        add     hl,de
        ret

