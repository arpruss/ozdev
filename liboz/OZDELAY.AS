        psect text

; void ozdelay(unsigned d)
global _ozdelay,$gint
_ozdelay:
        ld       hl,2
        add      hl,sp
        call     $gint
delaylp:
        dec     hl
        ld      a,h
        or      l
        jr      nz,delaylp
        ret


