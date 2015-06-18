psect text
global _ozgetclick,__ozclick_setting

_ozgetclick:
        ld      a,(__ozclick_setting)
        ld      l,a
        ld      h,0
        ret
