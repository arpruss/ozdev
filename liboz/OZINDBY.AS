global __ozindexbycounts,_ozgetindexby,_ozsetindexby
global _ozgetmemoeditmode,_ozsetmemoeditmode,_ozgetmemolastrecnum
global _ozsetmemolastrecnum,Model32k
global _ozgetappfont,_ozsetappfont,_ozgetstatus,restore_a000,set_a000_400

psect lowpage
page_in2:
        ld      a,Model32k
        or      a
        ld      a,8
        jr      nz,_32kmode
        ld      a,2
_32kmode:
        out     (3),a
        ld      a,4
        out     (4),a
        ret

_ozgetindexby:
        pop     hl
        pop     de
        push    de
        push    hl
        call    set_a000_400
        ld      hl,0bf25h
        ld      d,0
        add     hl,de
        ld      l,(hl)
        jp      restore_a000

_ozsetindexby:
;; ozsetindexby(byte typenum,byte indexby)
        pop     bc
        pop     de ;; typenum
        pop     hl ;; indexby
        push    hl
        push    de
        push    bc
        call    set_a000_400
        ld      a,l
        cp      1
        jp      c,restore_a000 ;; error
        ld      d,0
        ld      hl,__ozindexbycounts ;; in lowpage
        add     hl,de
        dec     a
        cp      (hl)
        jp      nc,restore_a000
        inc     a
        ld      hl,0bf25h
        add     hl,de
        ld      (hl),a
        jp      restore_a000

_ozgetmemoeditmode:
        call    set_a000_400
        ld      a,(0be51h)
        ld      l,a
        ld      h,0
        jp      restore_a000

_ozsetmemoeditmode:
        pop     hl
        pop     de
        push    de
        push    hl
        call    set_a000_400
        ld      a,e
        ld      (0be51h),a
        jp      restore_a000

_ozgetmemolastrecnum:
        call    set_a000_400
        ld      hl,(0be52h)
        jp      restore_a000

_ozsetmemolastrecnum:
        pop     de
        pop     hl
        push    hl
        push    de
        call    set_a000_400
        ld      (0be52h),hl
        jp      restore_a000

_ozgetappfont:
        call    page_in2
        ld      a,(0a05ch)
        ld      l,a
        ld      h,0
        jp      restore_a000

_ozsetappfont:
        pop     de
        pop     hl
        push    hl
        push    de
        call    page_in2
        ld      a,l
        ld      (0a05ch),a
        jp      restore_a000

_ozgetstatus:
        call    page_in2
        ld      a,(0a068h)
        ld      l,a
        ld      h,0
        jp      restore_a000

