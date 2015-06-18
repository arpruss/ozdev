global _ozfindnext,$TypeToCompare,$JumpToCompare,$DoRun

psect text
; unsigned ozfindnext(char type, unsigned *start_page, unsigned *start_offset)
_ozfindnext:
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      a,(ix)  ; type
        ld      ($TypeToCompare),a
        ld      a,nop
        ld      ($JumpToCompare),a
        ld      l,a
        ld      h,a
        ld      ($JumpToCompare+1),hl
        jp      $DoRun

