*include macros.inc

global _ozfindmemo
global $JumpToCompare
global $TypeToCompare
global $DoRun
global $MatchFilename
global $findbuffer

MemoTitleLength equ 20  ;; length of memo title

psect text
;; unsigned ozfindmemo(char *title, unsigned *page, unsigned *offset,
;;        unsigned *recnum);
_ozfindmemo:
        ld      a,jp
        ld      ($JumpToCompare),a
        ld      hl,$MatchFilename
        ld      ($JumpToCompare+1),hl
        ld      a,'M'
        ld      ($TypeToCompare),a
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      de,$findbuffer
        ld      bc,MemoTitleLength
        ld      l,(ix)
        ld      h,(ix+1)
        ldir

        xor     a
        ld      ($findbuffer+20),a
        ld      hl,$findbuffer
        ld      bc,MemoTitleLength
        cpir
        jp      nz,$DoRun
        dec     hl
        ld      a,c
        inc     a
ContinueSpaceFill:
        ld      (hl),' '
        inc     hl
        dec     a
        jr      nz,ContinueSpaceFill
        jp      $DoRun

