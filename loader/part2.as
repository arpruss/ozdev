global _Part2_routine,_end_Part2_routine,_Part2_central,_buffer
global _Part2_central_addr,_Part2_indir,_Part2_indir_addr

;;; it is essential that _Part2_indir-_Part2_routine should be <= 128 ;;;

psect   text
_Part2_routine:
;; put the central part of routine on stack
        ld      hl,0
        add     hl,sp
        ld      bc,_end_Part2_routine-_Part2_central
        or      a
        sbc     hl,bc
        ld      sp,hl
        push    hl
        ld      e,l
        ld      d,h
        ld      hl,0000
_Part2_central_addr equ $-2
        ldir
        pop     hl
        jp      (hl)

SAVED_FROM equ 0BFDEh+18-276

_Part2_central:
;; copy from 404 to 406 and 405 to 407
;; relocatable
        ld      c,1
        in      e,(c)
        inc     c
        in      d,(c)
        inc     c
        in      l,(c)
        inc     c
        in      h,(c)
        exx

;;;;;;; lower page = 406h

        ld      a,2
        out     (1),a
        ld      a,4
        out     (2),a

;;;;;;;; upper page = 409h

        ld      a,9
        out     (3),a
        ld      c,4
        out     (c),c

;;;
        ld      hl,_buffer+128  ;; adjusted buffer address
        ld      de,SAVED_FROM+128
        ld      bc,128
        ldir
;;;

;;;;;;;; upper page = 404h

        ld      a,4
        out     (3),a

        ld      de,08000h  ;; set destination offset
        ld      hl,0a000h  ;; set source offset
        ld      bc,02000h  ;; set length

        ldir

;;        ld      hl,0401h   ;; source page-4
        ld      c,1
        out     (c),c
;        ld      a,4
;        out     (2),a  ;; set source page

;;        ld      de,0407h  ;; destination page
        ld      a,7
        out     (3),a
;        ld      c,4
;        out     (c),c  ;; set destination page

        ld      d,0a0h     ;; set destination offset (e=00 from ldir,
        ld      h,080h  ;; set source offset (l=00 from ldir)
        ld      bc,02000h  ;; set length

        ldir

        exx
        out     (c),h
        dec     c
        out     (c),l
        dec     c
        out     (c),d
        dec     c
        out     (c),e

        ld      hl,_end_Part2_routine-_Part2_central
        add     hl,sp
        di
        ld      sp,hl
        jp      0000  ;; jp _Part2_indir
_Part2_indir_addr equ $-2

_Part2_indir:
        ei
        ret

_end_Part2_routine:
