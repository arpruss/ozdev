;; 16-bit multiply: mul hl,de
;; This is faster than the one Hi-Tech C provides

        psect   text
        global  amul,lmul,lmul$d0,lmul$h0

lmul$h0:
        ld      a,l
        ld      l,h    ;; h was zero so now l=0, so hl=0
        jp      NoHighByteLoaded
DZero:
lmul$d0:
        ld      a,e
        ex      de,hl
        ld      l,h  ;; remember that d was zero so now h=0, so hl=0
        jp      NoHighByteLoaded
amul:
lmul:
        xor     a
        cp      d
        jr      z,DZero
        ld      c,l
        ld      l,a    ; l=0
        ld      a,h
        ld      h,l    ; hl=0
        or      a
        jr      z,NoHighByte
        add     a,a
        jp      nc,1f
        ld      l,e
        ld      h,d
        add     hl,hl
1:
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
NoHighByte:
        ld      a,c
NoHighByteLoaded:
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        jp      nc,1f
        add     hl,de
1:
        add     hl,hl
        add     a,a
        ret     nc
        add     hl,de
        ret

