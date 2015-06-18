        psect   lowpage
        global  _ozputs_system,ScrCharSet

strlen  equ     7f0ch
DisplayStringAt  equ 7f4ch
;; Portable if ScrCharSet is portable ;;

; int ozputs_system(int x, int y, char *string);
set119:
        ld      bc,119
        ld      h,b
        ld      l,c
        jr      noset119
_ozputs_system:
        push    iy
        ld      iy,0
        add     iy,sp
        ld      l,(iy+8)
        ld      h,(iy+9)
        call    strlen
        ld      a,h
        or      a
        jr      nz,set119
        ld      a,l
        cp      120
        jr      c,set119
        ld      c,l
        ld      b,h
noset119:
        inc     l
        ex      de,hl
        ld      hl,0
        add     hl,sp
        sbc     hl,de
        ld      sp,hl     ; reserved hl bytes on stack

        ex      de,hl     ; de=destination
        ld      l,(iy+8)
        ld      h,(iy+9)  ; hl=source
        ldir              ; copy
        xor     a
        ld      (de),a    ; null terminate

        push    ix        ; save ix for Hi-Tech C
        ld      hl,2
        add     hl,sp
        push    hl
        ld      e,(iy+4)  ; x
        ld      d,(iy+5)
        ld      c,(iy+6)  ; y
        ld      b,(iy+7)
        ld      a,c
        cp      -1
        jr      z,LengthOnly

        call    DisplayStringAt
        pop     hl
        ld      hl,2
        add     hl,sp
        push    hl
LengthOnly:
        call    GetLength
        ld      c,(iy+4)
        ld      b,(iy+5)
        add     hl,bc
        pop     bc
        pop     ix
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
;   input:    on stack: address of string (not in 8000-9FFF)
;   returns:  HL=length of string
;   NOTE: clobbers ix
        ld      ix,0
        add     ix,sp
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

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
        ld      e,(ix+2)
        ld      d,(ix+3)   ; address of string
        ld      bc,0       ; subtotal
docount:
        ld      a,(de)
        or      a
        jr      z,done
        push    hl
        push    bc
        ld      c,a
        ld      b,0
        add     hl,bc
        add     hl,bc
        add     hl,bc
        ld      c,(hl)
        ld      b,0
        pop     hl      ; hl=subtotal
        add     hl,bc
        ld      c,l
        ld      b,h     ; bc=subtotal
        pop     hl      ; hl=address of string
        inc     de
        jr      docount
done:
        ld      l,c
        ld      h,b
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

