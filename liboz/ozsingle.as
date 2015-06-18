global _ozsinglepage,__ozauxstart

psect text
_ozsinglepage:
        ld      bc,0
        ld      (__ozauxstart),bc
        ret

psect beforebss
__ozauxstart:
        defw    2400

