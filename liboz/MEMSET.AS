psect text
global _memset
; memset(dest,value,count)
_memset:
        pop     hl
        ld      (retval),hl
        pop     de
        pop     hl
        pop     bc
        push    de  ;; save destination

        ld      a,c
        or      b
        jr      z,NothingToDo
        ld      a,l
        ld      (de),a
        dec     bc
        ld      a,c
        or      b
        jr      z,NothingToDo

        ld      l,e
        ld      h,d
        inc     de
        ldir

NothingToDo:
        ex      (sp),hl  ;; get destination pointer
        push    hl
        push    hl       ;; fill up stack
        jp      0        ;;
retval  equ     $-2      ;; self-modifying return

