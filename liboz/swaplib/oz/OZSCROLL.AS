        psect lowpage
; void ozscroll(int numbytes);
global _ozscroll,$gint
_ozscroll:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

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
       ld      de,0a000h
       ex      de,hl
       add     hl,bc
       push    hl
       pop     bc
       ld      de,0a000h
       call    7f00h ;24c4h
       pop     bc

       pop     bc

       ld      a,c
       out     (3),a
       ld      a,b
       out     (4),a
       ret

; void ozscrollclear(int numbytes);
global _ozscrollclear
_ozscrollclear:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      hl,0a000h+300
        ld      de,0a000h
        ld      bc,2400-300
	ldir
        ld      de,300
        xor     a
        ld      e,a
        ld      hl,0a000h+2400-300
        ld      bc,300
rpt:
        ld      (hl),e
        inc     hl
        dec     bc
        ld      a,b
        or      c
        jr      nz,rpt

        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret


