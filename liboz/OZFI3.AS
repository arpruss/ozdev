*include macros.inc

psect text
global $MatchFileID770
global $filenamelength
global $TypeToCompare,$JumpToCompare,$DoRun,$findbuffer
global $MatchCFileName770
global $filenamelength
global _ozfindfileid,_ozfindfilename

CFileNameLength equ 23

; unsigned ozfindfileid(unsigned file_id, unsigned *start_page, unsigned *start_offset)
_ozfindfileid:
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      a,'c'  ; type
        ld      ($TypeToCompare),a
        ld      a,jp
        ld      ($JumpToCompare),a
        ld      hl,$MatchFileID770
        ld      ($JumpToCompare+1),hl
        jp      $DoRun

; unsigned ozfindfilename(char *filename, unsigned *start_page, unsigned *start_offset)
_ozfindfilename:
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      a,'c'  ; type
        ld      ($TypeToCompare),a
        ld      a,jp
        ld      ($JumpToCompare),a
        ld      hl,$MatchCFileName770
        ld      ($JumpToCompare+1),hl
        ld      de,$findbuffer
        ld      l,(ix)
        ld      h,(ix+1)
        push    hl
        ld      bc,CFileNameLength
        ldir
        pop     hl
        call    strlen
        ld      a,l
        ld      ($filenamelength),a
        jp      $DoRun


