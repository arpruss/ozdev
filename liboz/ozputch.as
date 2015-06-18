        psect   lowpage
        global  _ozputch_system,ScrCharSet

strlen  equ     7f0ch
DisplayCharAt  equ 7f49h
;; Portable if ScrCharSet is portable ;;

; int ozputch(int x, int y, char c);
_ozputch_system:
        push    iy
        ld      iy,0
        add     iy,sp

        ld      c,(iy+6)  ; y
        ld      b,(iy+7)
        ld      a,c
        cp      -1
        jr      z,GetLength
        ld      e,(iy+4)  ; x
        ld      d,(iy+5)
        ld      l,(iy+8)
        ld      h,0
        push    hl
        call    DisplayCharAt
        ld      sp,iy
        pop     iy
        ret

PC_PLANE:
        ld      a,0eh
        ld      hl,0a000h
        jr      set_ozlen_page
PC_LARGE:
        ld      a,0eh
        ld      hl,0ab10h
        jr      set_ozlen_page
EO7PLANE:
        ld      a,0fh
        ld      hl,0a000h
        jr      set_ozlen_page
GetLength:
        in      a,(3)
        ld      e,a
        in      a,(4)
        ld      d,a

        ld      a,(ScrCharSet)
        cp      1
        jr      z,PC_PLANE
        or      a
        jr      z,PC_LARGE
        cp      3
        jr      z,EO7PLANE
;EO7LARGE
        ld      a,0fh
        ld      hl,0ab10h
set_ozlen_page:
        out     (3),a
        xor     a
        out     (4),a
        ld      a,(iy+8) ; character
        ld      c,a
        ld      b,0
        add     hl,bc
        add     hl,bc
        add     hl,bc
        ld      c,(hl)
        ld      b,0
        ld      l,c
        ld      h,b
        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a
        ld      c,(iy+4)
        ld      b,(iy+5)
        add     hl,bc
        ld      sp,iy
        pop     iy
        ret

