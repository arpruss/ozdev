*include macros.inc

global _ozinitfs,_ozfindnexterased,_ozfindmemoerased,$pages
global _ozinclocation,_ozloadcluster
global $pages,$cdir_pages,fs_init,$is770,_ozis770
psect lowpage
LEN equ 20


findbuffer: defs LEN
        defb 00

LastRecNum: defs 2

;; unsigned ozfindmemo(char *title, unsigned *page, unsigned *offset,
;;        unsigned *recnum);
_ozfindmemoerased:
        ld      a,jp
        ld      (JumpToCompare),a
        ld      hl,MatchFilename
        ld      (JumpToCompare+1),hl
        ld      a,'M'
        ld      (TypeToCompare),a
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      de,findbuffer
        ld      bc,LEN
        ld      l,(ix)
        ld      h,(ix+1)
        ldir

        ld      a,0
        ld      hl,findbuffer
        ld      bc,LEN
        cpir
        jr      nz,DoRun
        dec     hl
        ld      a,c
        inc     a
ContinueSpaceFill:
        ld      (hl),' '
        inc     hl
        dec     a
        jr      nz,ContinueSpaceFill
        jp      DoRun

; unsigned ozfindnext(char type, unsigned *start_page, unsigned *start_offset)
_ozfindnexterased:
        push    iy
        push    ix
        ld      ix,6
        add     ix,sp
        ld      a,(ix)  ; type
        ld      (TypeToCompare),a
        ld      a,nop
        ld      (JumpToCompare),a
        ld      hl,0
        ld      (JumpToCompare+1),hl

DoRun:
        ld      a,(fs_init)
        or      a
        call    z,_ozinitfs

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        xor     a
        or      b
        jr      nz,pageOK
        ld      iy,0100h
        ld      hl,0a008h  ;; 0a010h on OZ770
CDirStartOffset equ $-2           ;; selfmod
        jr      NotSameBlock
pageOK:
        push    bc
        pop     iy
        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        ld      de,8      ;; ,16 on OZ770
RecordLen2      equ $-2   ;; selfmod
        add     hl,de
        ld      a,h
        cp      0c0h
        jr      nz,NotSameBlock
        jr      IncPage
continue:
        ld      a,h
        cp      0c0h
        jp      c,searchloop
IncPage:
        undoc_iy
        inc     l    ; inc iyl
        undoc_iy
        ld      a,l  ; ld a,iyl
        cp      8
        jr      c,SameBlock
        undoc_iy
        ld      l,0  ; ld iyl,0
        undoc_iy
        inc     h    ; inc iyh
        ld      hl,0a008h    ;; 0a010h on OZ770
CDirStartOffset2 equ $-2
        jr      NotSameBlock
SameBlock:
        ld      hl,0a000h
NotSameBlock:
        push    hl
        undoc_iy
        ld      e,h  ; ld e,iyh
        ld      d,0
        ld      hl,$cdir_pages-2
        add     hl,de
        add     hl,de
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        or      h
        jp      z,NotFound
        undoc_iy
        ld      e,l      ; ld e,iyl
        add     hl,de    ; (d=0 still)
        ld      c,l
        ld      b,h      ; bc=current directory page
        pop     hl
searchloop:
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
OZ770jp:
        nop              ; modified
        nop              ; to jp OZ770Search
        nop              ; when running on OZ770 unit
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        ld      a,e
        and     d
        cp      -1
        jp      z,IncPage ;; was NotFound2
        ld      a,e
        or      d
        jp      nz,erased
        ld      (LastRecNum),de
        inc     hl
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
        ld      a,e
        or      d
        jp      z,CheckForm2
        inc     hl
        inc     hl
        inc     hl
        inc     hl
        jp      SearchIt
CheckForm2:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
        ld      a,e
        or      d
        jp      z,CheckForm3
        inc     hl
        inc     hl
        jp      SearchIt
CheckForm3:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        inc     hl
SearchIt:
        push    hl
        push    bc
        push    de
        ld      c,d
        ld      b,0
        ld      d,b
        ld      hl,$pages-2
        add     hl,bc
        add     hl,bc
        ld      a,(hl)
        out     (3),a
        inc     hl
        ld      a,(hl)
        out     (4),a
        ld      l,e
        ld      h,0
;        ld      hl,LogicalToPhysicalCluster - 1   ; translate Logical to Physical cluster number
;        add     hl,de
;        pop     de
;        ld      e,(hl)   ; PhysicalClusterNumber
;        push    de       ; save PhysicalClusterNumber in location
;        ld      d,0
;        ex      de,hl    ; hl=PhysicalClusterNumber
        add     hl,hl
        add     hl,hl
        add     hl,hl
        add     hl,hl
        add     hl,hl
        add     hl,hl     ; hl=64*ClusterNum
        ld      de,0a102h+2
        add     hl,de     ; hl=0a102h+40h*ClusterNum
        ld      a,(hl)
        cp      'M'       ; selfmod
TypeToCompare   equ $-1   ;
        jp      nz,Scontinue
JumpToCompare:                 ;;
        jp      MatchFilename  ;; selfmod
Matched:
        ld      a,(ix-8)
        out     (3),a
        ld      a,(ix-7)
        out     (4),a
        pop     de
        pop     bc
        undoc_iy
        ld      c,l  ; ld c,iyl
        undoc_iy
        ld      b,h  ; ld b,iyh
        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      (hl),c
        inc     hl
        ld      (hl),b
        pop     hl
        ld      bc,8  ;; ,16 on OZ770
RecordLength    equ $-2 ;; selfmod
        or      a
        sbc     hl,bc
        ld      c,l
        ld      b,h
        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      (hl),c
        inc     hl
        ld      (hl),b
        ld      l,(ix+6)
        ld      h,(ix+7)
        ld      bc,(LastRecNum)
        ld      (hl),c
        inc     hl
        ld      (hl),b
        ex      de,hl
        pop     bc   ;; already restored page
        pop     ix
        pop     iy
        ret
erased:
        ld      de,7
        add     hl,de
        jp      continue
Scontinue:
        pop     de
        pop     bc
        pop     hl
        jp      continue

erased770:
        ld      de,15
        add     hl,de
        jp      continue

OZ770Search:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        ld      a,e
        or      d
        jr      nz,erased770
        ld      a,e
        and     d
        cp      -1
        jp      z,IncPage
        ld      (LastRecNum),de
        ld      de,15
        add     hl,de      ; point to next record
        push    hl
        dec     hl         ; point to last part of record
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 14 into record
        ld      a,e
        and     d
        cp      -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 12 into record
        ld      a,e
        and     d
        cp      -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 10 into record
        ld      a,e
        and     d
        cp      -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 8 into record
        ld      a,e
        and     d
        cp     -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 6 into record
        ld      a,e
        and     d
        cp      -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        dec     hl
        ld      e,(hl)     ; hl=offset 4 into record
        ld      a,e
        and     d
        cp      -1
        jr      nz,OZ770PopAndSearch
        dec     hl
        ld      d,(hl)
        ld      a,d
        cp      -1
        jp      z,OZ770PopAndSkip
        dec     hl
        ld      e,(hl)     ; hl=offset 2 into record
OZ770PopAndSearch:
        pop     hl
        jp      SearchIt
OZ770PopAndSkip:
        pop     hl
        jp      continue

NotFound:
        pop     hl
NotFound2:
        ld      l,0
        ld      h,l
CleanUp:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        pop     ix
        pop     iy
        ret
MatchFilename:
        ld      de,20
        add     hl,de
        ld      de,findbuffer
        call    strcmp
        ld      a,l
        or      h
        jp      nz,Scontinue
        jp      Matched

