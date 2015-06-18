;; Portable if ScrCharSet is portable ;;

        psect   text
        global  _ozfont,ScrCharSet,_ozgetfont,_ozsetfont,__ozfontniceheight

; void ozfont(int font)
_ozsetfont:
_ozfont:
        ld      hl,2
        add     hl,sp
        ld      a,(hl)
        ld      (ScrCharSet),a
        and     1
        ld      a,13
        jr      z,large
        ld      a,10
large:
        ld      (__ozfontniceheight),a
        ret

_ozgetfont:
        ld      a,(ScrCharSet)
        ld      l,a
        ld      h,0
        ret

        psect   data
__ozfontniceheight:
        defb    10
