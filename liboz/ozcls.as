        psect lowpage
        global _ozcls,__ozactivepage,restore_a000

; void ozcls(void)
_ozcls:
        ld      bc,(__ozactivepage)
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      hl,0a000h
        ld      (hl),l   ;; l=0
        ld      de,0a001h
        ld      bc,2400-1
        ldir

        jp      restore_a000

