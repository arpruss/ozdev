global _ozgetauxsize,__ozauxstart

psect text
_ozgetauxsize:
        ld      hl,16384
        ld      bc,(__ozauxstart)
        or      a
        sbc     hl,bc
        ret

