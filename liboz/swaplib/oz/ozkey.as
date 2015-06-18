psect text
KeyBufGetPos   EQU 0c031h
KeyBufPutPos   EQU 0c032h
KeyboardBuffer EQU 0c033h
EnableKeyboard EQU 0c00fh

global _ozkeyhit
_ozkeyhit:
    ld  a,(EnableKeyboard)
    and 0ffh-6
    ld  (EnableKeyboard),a

    ld  a,(KeyBufPutPos)
    ld  b,a
    ld  a,(KeyBufGetPos)
    cp  b
    jr  z,empty
    ld  hl,1
    ret
empty:
    ld  hl,0
    ret

global _ozgetch
_ozgetch:
    push iy
    ld  a,(EnableKeyboard)
    and 0ffh-6
    ld  (EnableKeyboard),a
WaitForKey:
    ld      a,(KeyBufPutPos)
    ld      c,a
    ld      a,(KeyBufGetPos)
    cp      c
    jr      z,WaitForKey
    inc     a
    cp      0ch
    jr      c,dontzero
    xor     a
dontzero:
    ld      c,a
    ld      b,0
    ld      iy,KeyboardBuffer
    sla     c
    add     iy,bc
    ld      l,(iy)
    ld      h,(iy+1)
    ld      (KeyBufGetPos),a
    pop     iy
    ret

