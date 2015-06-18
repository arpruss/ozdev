global __ozputsprite,__ozactivepage

psect lowpage
__ozputsprite:
;; __ozputsprite(byte x,byte y,byte height,byte *sprite)
pop     hl
ld      c,3
in      e,(c)
inc     c
in      d,(c)
ld      bc,(__ozactivepage)
ld      a,c
out     (3),a
ld      a,b
out     (4),a
ld      ($ix_save),ix
ld      ($iy_save),iy

exx     ;; save initial screen stuff


pop     de  ; y
pop     bc  ; x
ld      b,e
call    getscreenptr
pop     bc  ; height
ld      b,c
pop     ix  ; sprite
push    bc  ; clean up stack
push    bc
push    bc
push    bc

call    putsprite

exx     ;; restore initial screen
ld      ix,0
$ix_save equ $-2
ld      iy,0
$iy_save equ $-2
ld      c,3
out     (c),e
inc     c
out     (c),d
jp      (hl)

;; By Benjamin Green

; sprite format:
; (byte image, byte mask)*height
; image mask
; 0     0    = white
; 1     0    = black
; 0     1    = transparent
; 1     1    = invert


putsprite: ; draws sprite (address IX, height B) at (address IY, bit A)

ld c,a
and a
jp z,_aligned

_yloop: push bc
ld l,(ix+1)
ld h,0FFh
ld b,c
scf
_shl1: rl l
rl h
djnz _shl1
ld a,(iy)
and l
ld e,a
ld a,(iy+1)
and h
ld d,a
ld l,(ix)
ld h,0
ld b,c
_shl2: sla l
rl h
djnz _shl2
ld a,e
xor l
ld (iy),a
ld a,d
xor h
ld (iy+1),a
inc ix
inc ix
ld de,30
add iy,de
pop bc
djnz _yloop
ret

_aligned: ld de,30
_a_yloop: ld l,(ix+1)
ld a,(iy)
and l
ld l,(ix)
xor l
ld (iy),a
inc ix
inc ix
add iy,de
djnz _a_yloop
ret

getscreenptr: ; converts (B,C) into screen location IY, bit offset A
;;push de
;;push hl
ld h,0
ld l,c
sla l
ld d,h
ld e,l
add hl,hl
add hl,hl
add hl,hl
add hl,hl
;;cp a
sbc hl,de
ld d,0A0h ; for page at A000
ld e,b
srl e
srl e
srl e
add hl,de
push hl
pop iy
ld a,b
and 7
;;pop hl
;;pop de
ret

