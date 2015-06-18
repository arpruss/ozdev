global _ozflashwrite,_ozflasherase,_ozflashread,_ozgetmembyte

psect lowpage

writeflash:
;; flash contents of register e to a-space address bc:hl ;;
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

        di
        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
        LD      (hl),40h                ; WRITE BYTE COMMAND
        LD      (hl),e                  ; WRITE VALUE
;        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
;        LD      (hl),70h                ; READ STATUS COMMAND
WAIT_FOR_READY:
        bit     7,(hl)                  ; READ THE STATUS BYTE, TEST FOR READY
        JR Z,WAIT_FOR_READY
        ld      (hl),050h               ; clear status
        ld      (hl),0ffh               ; RESET FLASH CONTROLLER
        ei

        ld      l,(hl)
        ld      h,0

        pop     af
        out     (3),a
        pop     af
        out     (4),a

        ret

eraseflash:
;; erase flash block at a-space address bc:hl ;;
        ld      a,b
        cp      2
        ret     nc  ;; page too big
        ld      a,b
        cp      1
        jp      z,xpage01xx
        ld      a,c
        cp      48h
        ret     c  ;; page too small
        jp      xpagesafe
xpage01xx:
        ld      a,c
        cp      80h
        ret     nc  ;; page number too big
xpagesafe:
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

        di
        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
        LD      (hl),20h                ; BLOCK ERASE COMMAND
        LD      (hl),0D0h                  ; WRITE VALUE
;        LD      (hl),0ffh               ; RESET FLASH CONTROLLER
;        LD      (hl),70h                ; READ STATUS COMMAND
xWAIT_FOR_READY:
        bit     7,(hl)                  ; READ THE STATUS BYTE
        JR Z,xWAIT_FOR_READY
        ld      (hl),050h               ; clear status
        ld      (hl),0ffh               ; RESET FLASH CONTROLLER
        ei

        pop     af
        out     (3),a
        pop     af
        out     (4),a

        ret

readflash:
        in      a,(4)
        push    af
        in      a,(3)
        push    af   ;; save starting page
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      l,(hl)

        pop     af
        out     (3),a
        pop     af
        out     (4),a

        ret


psect   text
global _ozflashwrite
; byte ozflashwrite(unsigned page, unsigned offset, byte value);
; offsets range between 0 and 8191
_ozflashwrite:
        push    ix
        ld      ix,4
        add     ix,sp
        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      bc,0a000h
        add     hl,bc
        jr      c,GetOut
        ld      e,(ix+4) ; value
        ld      c,(ix+0)
        ld      b,(ix+1)
        call    writeflash
GetOut:
        pop     ix
        ret

;; byte ozflashread(unsigned page, unsigned offset)
;; byte ozgetmembyte(unsigned page, unsigned offset)
_ozgetmembyte:
_ozflashread:
        push    ix
        ld      ix,4
        add     ix,sp
        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      bc,0a000h
        add     hl,bc
        jr      c,GetOut2
        ld      c,(ix+0)
        ld      b,(ix+1)
        call    readflash
GetOut2:
        pop     ix
        ret


_ozflasherase:
; byte ozflasherase(unsigned page);  /* erases the block containing the page */
        push    ix
        ld      ix,4
        add     ix,sp
;        ld      e,(ix+4)
;        ld      l,(ix+2)
;        ld      h,(ix+3)
        ld      hl,0a000h
        ld      c,(ix+0)
        ld      b,(ix+1)
        call    eraseflash
        pop     ix
        ret
