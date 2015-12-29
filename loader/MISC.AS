psect lowpage

global _copypage

global _buffer
_buffer: defs 256

_copypage:  ; copypage(unsigned dest, unsigned src)

        pop     hl
        pop     bc  ;; dest
        pop     de  ;; src
        push    de
        push    bc
        exx

        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        push    de

        ld      b,8192/256
        ld      hl,0a000h
        ld      e,l
        ld      d,h
LoopTop:
        push    bc
        push    de
        ld      de,_buffer
        exx
        ld      a,e
        out     (3),a
        ld      a,d
        out     (4),a  ;; source page
        exx
        ld      bc,256
        ldir
        pop     de
        push    hl
        ld      bc,256
        ld      hl,_buffer
        exx
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a  ;; dest page
        exx
        ldir
        pop     hl
        pop     bc
        djnz    LoopTop

        pop     de
        ld      c,3
        out     (c),e
        inc     c
        out     (c),d

        exx
        jp      (hl)

writeflash256:
;; flash _buffer to a-space address bc:hl ;;
        ld      a,b
        cp      2
        ret     nc  ;; page too big
        ld      a,b
        cp      1
        jp      z,page01xx
        ld      a,c
        cp      48h
        ret     c  ;; page too small
        jp      pagesafe
page01xx:
        ld      a,c
        cp      80h
        ret     nc  ;; page number too big
pagesafe:
        ld      a,h
        cp      0a0h
        ret     c   ;; offset too small
        cp      0c0h
        ret     nc  ;; offset too big
;;; we can now safely access the flash ;;
        in      a,(4)
        push    af
        in      a,(3)
        push    af   ;; save starting page
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      de,_buffer
        ld      b,0    ;; 256 bytes
FlashLoop:
        di
        ld      a,(de)
        inc     de
        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
        LD      (hl),40h                ; WRITE BYTE COMMAND
        LD      (hl),a                  ; WRITE VALUE
;        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
;        LD      (hl),70h                ; READ STATUS COMMAND
WAIT_FOR_READY:
        bit     7,(hl)                  ; READ THE STATUS BYTE, TEST FOR READY
        JR      Z,WAIT_FOR_READY
        ld      (hl), 50h               ; CLEAR STATUS
        ld      (hl), 0FFh              ; READ ARRAY / RESET
        inc     hl
        ei

        djnz    FlashLoop

        ld      l,(hl)
        ld      h,0

        pop     af
        out     (3),a
        pop     af
        out     (4),a

        ret

global _ozflashwrite256
; byte ozflashwrite256(unsigned page, unsigned offset);
; offsets range between 0 and 8191
_ozflashwrite256:
        push    ix
        ld      ix,4
        add     ix,sp

        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      bc,0a000h
        add     hl,bc
        jr      c,GetOut
        ld      c,(ix+0)
        ld      b,(ix+1)
        call    writeflash256
GetOut:
        pop     ix
        ret

global _ozflashread256
; void ozflashread256(unsigned page, unsigned offset);
; offsets range between 0 and 8191
_ozflashread256:
        pop     hl
        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        exx
        pop     bc       ;; page
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     hl       ;; offset
        push    hl
        push    bc
        ld      bc,0a000h
        add     hl,bc
        ld      de,_buffer
        ld      bc,256
        ldir
        exx
        out     (c),d
        dec     c
        out     (c),e
        jp      (hl)

global _ozmemwrite256,_blk_size
; void ozmemwrite256(unsigned page, unsigned offset);
; offsets range between 0 and 8191
_ozmemwrite256:
        pop     hl
        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        exx
        pop     bc       ;; page
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     hl       ;; offset
        push    hl
        push    bc
        ld      bc,0a000h
        add     hl,bc
        ld      de,_buffer
        ex      de,hl
        ld      bc,256
_blk_size equ $-2
        ldir
        exx
        out     (c),d
        dec     c
        out     (c),e
        jp      (hl)

global _theline
_theline:
        ld      c,3
        in      e,(c)
        inc     c
        in      d,(c)
        ld      hl,400h
        out     (c),h
        dec     c
        out     (c),l

        exx

        ld      hl,0a000h+9*30
        ld      (hl),255
        ld      e,l
        ld      d,h
        inc     de
        ld      bc,28
        ldir
        ld      a,07fh
        ld      (de),a

        exx

        out     (c),e
        inc     c
        out     (c),d

        ret
