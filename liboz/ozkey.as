psect text

global KeyBufGetPos,KeyBufPutPos,KeyboardBuffer,EnableKeyboard


global _ozkeyhit
_ozkeyhit:
    ld  de,EnableKeyboard
    ld  a,(de)
    and 0ffh-7
    ld  (de),a

    ld  hl,KeyBufGetPos
    ld  a,(hl)
    inc hl      ;; KeyBufPutPos
    cp  (hl)
    ld  hl,1
    ret nz
    dec hl
    ret

global _ozkeyclear
_ozkeyclear:
        ld      hl,KeyBufPutPos
        ld      a,(hl)
        dec     hl    ;; KeyBufGetPos
        ld      (hl),a
	ret

global __ozgetch
__ozgetch:
    ld	    hl,KeyBufGetPos
    ld      de,EnableKeyboard
    jr	    WaitForKey
WaitForKey0:
    dec     hl
    halt
WaitForKey:
    ld     a,(de)
    and    0ffh-7
    ld      (de),a
    ld	    a,(hl)   ;; KeyBufGetPos
    inc     hl
    cp	    (hl)     ;; KeyBufPutPos
    jr      z,WaitForKey0
    inc     a
    cp	    0ch
    jr	    c,dontzero
    xor     a
dontzero:
    ld	    c,a
    sla     c
    ld	    b,0
    inc     hl	     ;; KeyboardBuffer
    add     hl,bc
    ld	    c,(hl)
    inc     hl
    ld	    h,(hl)
    ld	    l,c
    ld	    (KeyBufGetPos),a
    ret

global _ozungetch
_ozungetch:

    pop    hl
    pop    bc
    push   bc
    push   hl

global putinbuf_noclick
putinbuf_noclick:
        ld      a,(KeyBufPutPos)
        inc     a
        cp      0ch
        jr      c,dontzerox
        xor     a
dontzerox:
        ld      l,a
        ld      a,(KeyBufGetPos)
        cp      l
        jp      z,KBufferFull

        ld      a,(KeyBufPutPos)
        inc     a
        cp      0ch
        jr      c,dontzero2
        xor     a
dontzero2:
        ld      (KeyBufPutPos),a
        ld      e,a
        ld      d,0
        ld      hl,KeyboardBuffer
        add     hl,de
        add     hl,de
        ld      (hl),c
        inc     hl
        ld      (hl),b
KBufferFull:
        ret

