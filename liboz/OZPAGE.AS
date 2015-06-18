global _ozsetactivepage, _ozsetdisplaypage, _ozgetactivepage
global _ozgetdisplaypage, _ozdisplayactivepage, _ozactivatedisplaypage
global __ozactivepage,_ozswapactivedisplay

;;
;; currently, there is support for two display pages, one at 400h and
;; the other at 404h
;;

psect text
_ozsetactivepage:
;; void ozsetactivepage(byte page)
        pop     hl
        pop     bc
        push    bc
        ld      a,c
        or      a
        jr      nz,PageOne
        xor     a
        ld      (__ozactivepage),a
;; assume high byte is 4
        jp      (hl)
PageOne:
        ld      a,4
        ld      (__ozactivepage),a
;; assume high byte is 4
        jp      (hl)

_ozgetactivepage:
        ld      a,(__ozactivepage)
        or      a
        jr      z,PageZero
        ld      hl,1
        ret
PageZero:
        ld      l,a
        ld      h,a  ; hl=0
        ret

_ozsetdisplaypage:
        pop     hl
        pop     bc
        push    bc
        ld      a,c
        or      a
        jr      nz,PageOneDisp
        xor     a
        out     (22h),a
;        ld      a,4
;        out     (23h),a      ;; removed as always 4
        jp      (hl)
PageOneDisp:
        ld      a,4
        out     (22h),a
;        out     (23h),a      ;; removed as always 4
        jp      (hl)

_ozgetdisplaypage:
        in      a,(22h)
        or      a
        jr      z,PageZero
        ld      hl,1
        ret

_ozdisplayactivepage:
        ld      a,(__ozactivepage)
        out     (22h),a
;       ld      a,(__ozactivepage+1)
;       out     (23h),a
        ret

_ozactivatedisplaypage:
        in      a,(22h)
        ld      (__ozactivepage),a
;       in      a,(23h)
;       and     0fh
;       ld      (__ozactivepage+1),a
        ret

_ozswapactivedisplay:
        ld      hl,__ozactivepage
        ld      b,(hl)
        in      a,(22h)
        ld      (hl),a
        ld      a,b
        out     (22h),a
        ret
