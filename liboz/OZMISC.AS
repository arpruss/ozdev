psect text
global $gint
; fetch int from (HL)
$gint:
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        ret

