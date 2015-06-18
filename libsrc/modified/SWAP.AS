psect   text
global _swap_chars
_swap_chars:
        push    ix
        push    iy
        ld      ix,6
        add     ix,sp
        ld      l,(ix)
        ld      h,(ix+1)
        ld      e,(ix+2)
        ld      d,(ix+3)
        ld      iy,0
        add     iy,de
        ld      c,(ix+4)
        ld      b,(ix+5)
        ld      a,b
        or      c
        jp      z,done
continue:
        ld      a,(hl)
        ld      e,(iy)
        ld      (hl),e
        ld      (iy),a
        inc     hl
        inc     iy
        dec     bc
        ld      a,b
        or      c
        jp      nz,continue
done:
        pop     iy
        pop     ix
        ret
