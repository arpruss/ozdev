        psect text
        global   _ozsetlight,__ozbacklight,_oztogglelight ;;,_ozgetlight
        global   $ozlcdstatus,$ozsetlcdstate ;; ,__ozbacklight_save



; void oztogglelight(void)
_oztogglelight:
        ld      a,(__ozbacklight)
        and     040h
        xor     040h
        ld      l,a
        push    hl
        call    _ozsetlight
        pop     hl
        ret

; void ozsetlight(char light)
_ozsetlight:
        pop     hl
        pop     bc
        push    bc
        push    hl
        ld      a,c
        or      a
        ld      hl,__ozbacklight
        jr      nz,LightOn
        ld      a,(hl)
        and     0ffh - 040h
        ld      (hl),a

        ld      hl,($ozlcdstatus)
        ld      a,h
        and     0feh
        jr      DoSet

;        ld      hl,__ozbacklight_save
;        ld      a,(hl)
;        and     0ffh - 08h
;        ld      (hl),a
LightOn:
        ld      a,040h
        or      (hl)
        ld      (hl),a
;        ld      hl,__ozbacklight_save
;        ld      a,08h
;        or      (hl)
;        ld      (hl),a
        ld      hl,($ozlcdstatus)
        ld      a,h
        or      01
DoSet:
        ld      h,a
        push    hl
        call    $ozsetlcdstate
        pop     hl
        ret

