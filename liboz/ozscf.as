global _ozscancustomfonts,set_save_pagenum,restore_a000,__ozfontpointers
global _ozactivecustomfont
global header0,header1

psect lowpage

_ozactivecustomfont: defb 0ffh

scanfont:
;; scan font at hl
;; returns: a=0 if no font, a=1 if font
;;   correctly sets header at de if font, else value of header is
;;   undefined
        ld      (activenum),a
        push    hl
        push    de
        call    set_save_pagenum
        pop     de
        pop     hl

        ld      a,c
        ld      (de),a
        inc     de
        ld      a,b
        ld      (de),a
        inc     de
        inc     de
        inc     de
        inc     de
        inc     de

        ld      bc,8
        add     hl,bc
        ld      a,(hl)  ;; height
        ld      (de),a

        dec     hl  ;; last
        ld      a,(hl)
        inc     de
        ld      (de),a ;; last char

        dec     hl
        dec     hl
        ld      b,(hl)
        dec     hl
        ld      c,(hl)  ;; bc=length
        dec     bc
        dec     bc      ;; adjust for checksum
        dec     hl

        ld      a,(hl)
        cp      46h
        jr      nz,NotFound
        dec     hl
        ld      a,(hl)
        cp      6fh
        jr      nz,NotFound
;        dec     hl
;        dec     hl      ;; magic

;;        jp      inactive

        push    hl

        push    ix

        push    hl
        pop     ix

        ld      hl,0
        ld      d,h

SumUp:
        ld      a,c
        or      b
        jr      z,Summed
        ld      e,(ix)
        add     hl,de
        inc     ix
        dec     bc
        jp      SumUp
Summed:

        pop     ix

        pop     de
        dec     de
        ld      a,(de)
        cp      h
        jr      nz,NotFound
        dec     de    ;; checksum
        ld      a,(de)
        cp      l
        jr      nz,NotFound

        ld      hl,11
        add     hl,de
        ld      a,(hl) ;; active
        or      a
        jr      nz,inactive
        ld      a,0
activenum equ $-1
        ld      (_ozactivecustomfont),a

inactive:
        call    restore_a000
        ld      a,1
        ret
NotFound:
        call    restore_a000
        xor     a
        ret

psect text

_ozscancustomfonts:
;; byte ozscancustomfonts();
;;   bit 0 of return: have font 0
;;   bit 1 of return: have font 1

        ld      a,0ffh
        ld      (_ozactivecustomfont),a

        ld      hl,0a400h
        ld      de,header0
        ld      a,4
        call    scanfont
        push    af
        ld      de,header0
        ld      hl,__ozfontpointers+8
        call    setheader

        ld      hl,0b180h
        ld      de,header1
        ld      a,5
        call    scanfont
        push    af
        ld      de,header1
        ld      hl,__ozfontpointers+10
        call    setheader
        pop     af
        add     a,a
        ld      b,a
        pop     af
        or      b

        ld      l,a


        ret

setheader:
        or      a
        jr      nz,doset
        ld      de,(__ozfontpointers+2)
doset:
        ld      (hl),e
        inc     hl
        ld      (hl),d
        ret

