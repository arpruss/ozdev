;; Portable if ScrCharSet is portable ;;

        psect   text
        global  _ozfont

ScrCharSet equ 0c066h
; void ozfont(int font)
_ozfont:
        ld      hl,2
        add     hl,sp
        ld      a,(hl)
        ld      (ScrCharSet),a
        ret

