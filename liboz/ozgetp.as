        psect lowpage
        global _ozgetpoint,__ozactivepage
; int ozgetpoint(int x, int y)
;; color can be 0 or 1
;; returns: offset of position, or -1 if out of bounds
_ozgetpoint:
        push    ix
        ld      ix,2
        add     ix,sp
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc
        ld      bc,(__ozactivepage)
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      a,(ix+5) ; y.high
        or      a
        jp      nz,GetOut0
        ld      a,(ix+3) ; x.high
        or      a
        jp      nz,GetOut0

        ld      a,(ix+4) ; y
        cp      80
        jp      nc,GetOut0
        ld      l,a
        ld      h,0
        add     hl,hl
        ld      c,l
        ld      b,h      ; bc=2*y
        add     hl,hl    ; hl=4*y
        add     hl,hl    ; hl=8*y
        add     hl,hl    ; hl=16*y
        add     hl,hl    ; hl=32*y
        sbc     hl,bc    ; hl=30*y

        ld      a,(ix+2) ; x
        cp      240
        jp      nc,GetOut0
        ld      b,a
        srl     a
        srl     a
        srl     a         ; a=x/8
        ld      e,a
        ld      d,0a0h
        add     hl,de     ; hl=offset
        ld      e,a
        ld      a,b
        and     07h
        ld      b,a
        ld      a,1
        jp      z,NoShift
RepeatShift:
        sla     a
        djnz    RepeatShift
NoShift:
        and     (hl)
        jp      nz,GetOut1

GetOut0:
        ld      hl,0
GetOut:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret
GetOut1:
        ld      hl,1
        jp      GetOut
