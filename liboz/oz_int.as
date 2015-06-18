global __ozsetisr

psect text
__ozsetisr:
        ld      a,(0)
        cp      di
        jr      nz,error

;; install an interrupt table ;;
        ld     hl,0fe00h
        ld     de,0fe01h
        ld     bc,256
        ld     a,0ffh
        ld     (hl),a
        ldir

        di
        ld      hl,0fff4h
        ld      (hl),di
        inc     hl
        ld      (hl),jp
        inc     hl

        pop     bc  ;; return address
        pop     de  ;; isr argument
        push    de
        push    bc

        ld      (hl),e
        inc     hl
        ld      (hl),d
        ld      a,jr
        ld      (0ffffh),a

        ld      a,0feh
        ld      i,a
        im      2
        ei
        ld      hl,0
        ret
error:
        ld hl,-1
        ret


