psect   lowpage
global __ozhline,__ozactivepage

orhl    equ     0b6h      ; or (hl)
andhl   equ     0a6h      ; and (hl)
xorhl   equ     0aeh      ; xor (hl)

; _ozhline(byte x, byte y, byte len, byte color)
__ozhline:
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

        ld      hl,Operation       ;; self-modifying code
        ld      a,(ix+8)
        bit     0,a
        jp      z,NotSet
        ld      (hl),nop           ;; BLACK
        inc     hl
        ld      (hl),orhl
        jp      SelfModDone
NotSet:
        bit     1,a
        jp      nz,IsXor
        ld      (hl),cpl           ;; WHITE
        inc     hl
        ld      (hl),andhl
        jp      SelfModDone
IsXor:
        ld      (hl),nop
        inc     hl
        ld      (hl),xorhl
SelfModDone:
        ld      l,(ix+4) ; y
        ld      h,0
        add     hl,hl    ; hl=2*y
        ld      c,l
        ld      b,h      ; bc=2*y
        add     hl,hl    ; hl=4*y
        add     hl,hl    ; hl=8*y
        add     hl,hl    ; hl=16*y
        add     hl,hl    ; hl=32*y
        sbc     hl,bc    ; hl=30*y

        ld      a,(ix+2) ; x
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
        ld      b,(ix+6)
        ld      de,30
        ld      c,a
InnerLoop:
        ld      a,c
Operation:
        nop                ;;
        or      (hl)       ;; self-modifying code
                           ;; varies between nop / or (hl) if color is
                           ;; BLACK, cpl / and (hl) if color is WHITE,
                           ;; and nop // xor (hl) if color is XOR
        ld      (hl),a
        rl      c
        jr      c,inc_hl
        djnz    InnerLoop
GetOut:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        ret
inc_hl:
        rl      c
        inc     hl
        djnz    InnerLoop
        jp      GetOut
GetOut0:
        ld      hl,-1
        jr      GetOut

