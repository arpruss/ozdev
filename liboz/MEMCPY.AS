psect text
global _memcpy,_blkcpy
; memcpy(dest,src,count)
_blkcpy:
_memcpy:
        pop     hl
        ld      (retval),hl
        pop     de
        pop     hl
        pop     bc
        push    de  ;; save destination

        ld      a,c
        or      b
        jr      z,NothingToDo

        ldir

NothingToDo:
        ex      (sp),hl  ;; get destination pointer
        push    hl
        push    hl       ;; fill up stack
        jp      0        ;;
retval  equ     $-2      ;; self-modifying return
