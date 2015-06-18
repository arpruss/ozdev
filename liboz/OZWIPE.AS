psect lowpage
global _ozinstallmemorywiper,$wipe_hook

towipe:
        defb 4,5,7,8,9
endwipe:

_ozinstallmemorywiper:
        ld      a,call
        ld      ($wipe_hook),a
        ld      de,wiper
        ld      ($wipe_hook+1),de
        ret

wiper:
        ld      b,endwipe-towipe
        ld      hl,towipe
top:
        ld      a,(hl)
        inc     hl
        call    dowipe
        djnz    top
        ret

dowipe:
        push    hl
        push    bc
        out     (3),a
        ld      hl,0a000h
        ld      bc,02000h-1
        ld      (hl),0
        ld      de,0a001h
        ldir
        pop     bc
        pop     hl
        ret
