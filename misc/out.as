        global  $gint
        psect text

;; Portable if ScrCharSet is portable ;;

strlen  equ     7f0ch
DisplayStringAt  equ 7f4ch
ScrCharSet equ 0c066h

; void ozscroll(int numbytes);
_ozscroll:
        in      a,(1)
        ld      c,a
        in      a,(2)
        ld      b,a
        push    bc

        ld      bc,03fch
        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a

       ld      hl,4
       add     hl,sp
       call    $gint
       push    hl
       pop     bc
       ld      de,2400
       ex      de,hl
       cp      a
       sbc     hl,bc
       push    hl
       ld      de,8000h
       ex      de,hl
       add     hl,bc
       push    hl
       pop     bc
       ld      de,8000h
       call    7f00h ;24c4h
       pop     bc

       pop     bc

       ld      a,c
       out     (1),a
       ld      a,b
       out     (2),a
       ret


; void ozdelay(int d)
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


; void kbdoff();
_kbdoff:
        in      a,(7)
        or      1
        out     (7),a
        ret

_kbdon:
        in      a,(7)
        and     0feh
        out     (7),a
        ret

; void ozbox(int x, int y, int width, int height);
_ozbox:
        ld      iy,0
        add     iy,sp
        ld      l,(iy+8) ; height
        ld      h,(iy+9)
        push    hl
        ld      l,(iy+6) ; width
        ld      h,(iy+7)
        push    hl
        ld      c,(iy+4) ; y
        ld      b,(iy+5)
        ld      e,(iy+2) ; x
        ld      d,(iy+3)
        call    07f46h ; 0x617e
        pop     bc
        pop     bc
        ret

; void ozorbyte(int offset, char byte);
;   or byte into the display at offset
_ozorbyte:
        ld      iy,0
        add     iy,sp
        in      a,(1)
        ld      c,a
        in      a,(2)
        ld      b,a
        push    bc

        ld      bc,03fch
        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a

        ld      l,(iy+2) ; offset
        ld      h,(iy+3)
        ld      bc,8000h
        add     hl,bc
        ld      a,(hl)
        or      (iy+4)   ; byte
        ld      (hl),a

        pop     bc

        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a
        ret

; int ozibyte(int offset);
;   get byte from the display at offset
_ozibyte:
        ld      iy,0
        add     iy,sp
        in      a,(1)
        ld      c,a
        in      a,(2)
        ld      b,a
        push    bc

        ld      bc,03fch
        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a

        ld      l,(iy+2) ; offset
        ld      h,(iy+3)
        ld      bc,8000h
        add     hl,bc
        ld      a,(hl)
        ld      l,a
        xor     a
        ld      h,a

        pop     bc

        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a
        ret


; void ozanbyte(int offset, char byte);
;   or byte into the display at offset
_ozanbyte:
        ld      iy,0
        add     iy,sp
        in      a,(1)
        ld      c,a
        in      a,(2)
        ld      b,a
        push    bc

        ld      bc,03fch
        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a

        ld      l,(iy+2) ; offset
        ld      h,(iy+3)
        ld      bc,8000h
        add     hl,bc
        ld      a,(hl)
        and     (iy+4)   ; byte
        ld      (hl),a

        pop     bc

        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a
        ret

; void ozfont(int font)
_ozfont:
        ld      iy,0
        add     iy,sp
        ld      a,(iy+2)
        ld      (ScrCharSet),a
        ret

; int ozput(int x, int y, char *string);
set119:
        ld      bc,119
        ld      h,b
        ld      l,c
        jr      noset119
_ozput:
        ld      iy,0
        add     iy,sp
        ld      l,(iy+6)
        ld      h,(iy+7)
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
        ld      l,(iy+6)
        ld      h,(iy+7)  ; hl=source
        ldir              ; copy
        xor     a
        ld      (de),a    ; null terminate

        push    ix        ; save ix for Hi-Tech C
        ld      hl,2
        add     hl,sp
        push    hl
        ld      e,(iy+2)  ; x
        ld      d,(iy+3)
        ld      c,(iy+4)  ; y
        ld      b,(iy+5)
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
        ld      c,(iy+2)
        ld      b,(iy+3)
        add     hl,bc
        pop     bc
        pop     ix
        ld      sp,iy
        ret

PC_PLANE:
        ld      a,0eh-4
        ld      hl,08000h
        jr      set_ozlen_page
PC_LARGE:
        ld      a,0eh-4
        ld      hl,08b10h
        jr      set_ozlen_page
EO7PLANE:
        ld      a,0fh-4
        ld      hl,08000h
        jr      set_ozlen_page
GetLength:
;   input:    on stack: address of string (not in 8000-9FFF)
;   returns:  HL=length of string
;   NOTE: clobbers ix
        ld      ix,0
        add     ix,sp
        in      a,(1)
        ld      c,a
        in      a,(2)
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
        ld      a,0fh-4
        ld      hl,08b10h
set_ozlen_page:
        out     (1),a
        xor     a
        out     (2),a
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
        out     (1),a
        ld      a,b
        out     (2),a
        ret

; void ozcls(void)
_ozcls:
        in      a,(1)
        ld      c,a
        in      a,(2)
        ld      b,a
        push    bc

        ld      bc,03fch
        ld      a,c
        out     (1),a
        ld      a,b
        out     (2),a

        ld      de,8000h
        ld      bc,2400
        ld      l,0
        call    7f12h

       pop     bc

       ld      a,c
       out     (1),a
       ld      a,b
       out     (2),a
       ret

        global  _ozcls
        global  _ozput
        global  _ozorbyte
        global  _ozanbyte
        global  _ozibyte
        global  _ozbox
        global  _kbdoff
        global  _kbdon
        global  _ozdelay
        global  _ozscroll
        global  _ozfont

