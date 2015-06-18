	psect	text
        global  _strchr

_strchr:
        pop     bc
        pop     hl    ; string
        pop     de    ; char
        push    de
        push    hl
        push    bc
top:
        ld      a,(hl)
        or      a
        jr      z,NotFound
        cp      e
        ret     z
        inc     hl
        jp      top
NotFound:
        ld      h,a  ;; recall that a=0 at this point
        ld      l,a
        ret

