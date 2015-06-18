	psect lowpage
; void ozscroll(unsigned numbytes);
global _ozscroll,__ozactivepage,restore_a000
_ozscroll:
        pop     hl
        exx

        ld      de,(__ozactivepage)

        ld      a,e
	out	(3),a
        ld      a,d
	out	(4),a

       ld      hl,0a000h

       pop     bc
       push    bc

       add     hl,bc

       ex      de,hl

       ld      hl,2400
       sbc     hl,bc
       ld      c,l
       ld      b,h   ;; length of move

       ex      de,hl

       ld      de,0a000h

       ldir

       call    restore_a000

       exx
       jp      (hl)

; void ozscrolldown(unsigned numbytes);
global _ozscrolldown,__ozactivepage
_ozscrolldown:
        pop     hl
        exx

        ld      de,(__ozactivepage)

        ld      a,e
	out	(3),a
        ld      a,d
	out	(4),a

        ld      hl,0a000h+2400-1

        pop     bc
        push    bc

        or      a
        sbc     hl,bc

        ex      de,hl

        ld      hl,2400
        sbc     hl,bc
        ld      c,l
        ld      b,h   ;; length of move

        ex      de,hl

        ld      de,0a000h+2400-1

        lddr

        call    restore_a000

        exx
        jp      (hl)

