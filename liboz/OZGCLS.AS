global _ozcls,_ozgreycls
global __ozactivepage

psect text
_ozgreycls:
        ld      a,4
        ld      (__ozactivepage),a
        call    _ozcls
        xor     a
        ld      (__ozactivepage),a
        jp      _ozcls

