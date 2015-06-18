global _ozinitfs
global __ozclusterbuffer
global $pages,$cdir_pages,fs_init,__ozmodel,_ozdetectmodel
global restore_a000
global $JumpToCompare
global $TypeToCompare
global $DoRun
global $MatchFilename
global $findbuffer
global $filenamelength
global $MatchFileID770
global $MatchCFileName770


MemoTitleLength equ 20  ;; length of memo title
CFileNameLength equ 23

*include macros.inc

psect lowpage

$findbuffer: defm '12345678901234567890123'
            defb 00

$filenamelength: defb 00

LastRecNum: defw 0


psect bss

is_init0:
        defs 1

psect text


init0:
        ld      a,1
        ld      (is_init0),a

        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel
        ld      a,(__ozmodel)
        and     2
        ret     z

;; assume is OZ770 and modify code to fit ;;
        ld      hl,0a010h
        ld      ($CDirStartOffset),hl
        ld      ($CDirStartOffset2),hl
        ld      a,jp
        ld      ($OZ770jp),a
        ld      hl,$OZ770Search
        ld      ($OZ770jp+1),hl
        ld      hl,16
        ld      ($RecordLength),hl
        ld      ($RecordLen2),hl
        ret





psect lowpage

$DoRun:
        ld      a,(fs_init)
        or      a
        call    z,_ozinitfs
        ld      a,(is_init0)
        or      a
        call    z,init0

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
$CDirStartOffset equ $-2           ;; selfmod
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
$RecordLen2      equ $-2   ;; selfmod
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
$CDirStartOffset2 equ $-2
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
$OZ770jp:
        nop              ; modified
        nop              ; to jp OZ770Search
        nop              ; when running on OZ770 unit
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        ld      a,e
        or      d
        jp      z,erased
        ld      a,e
        and     d
        cp      -1
        jp      z,IncPage ;; was NotFound2
;;        jp      z,NotFound2
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
        push    de      ;; Save LogicalClusterNumber of location
        ld      c,d
        ld      b,0
        ld      d,b
        ld      hl,$pages-1
        add     hl,bc
        ld      a,(hl)
        add     a,80h
        out     (3),a
        ld      a,b     ;; recall that b=0
        adc     a,b       ;; again, recall that b=0
        out     (4),a
        ld      hl,LogicalToPhysicalCluster - 1   ; translate Logical to Physical cluster number
        add     hl,de
;;        pop     de
        ld      e,(hl)   ; PhysicalClusterNumber
;;        push    de       ; save PhysicalClusterNumber in location
        ld      h,e
        ld      l,0       ; NOTE: carry is clear as add hl,de did not overflow
        rr      h
        rr      l
        rr      h
        rr      l         ; hl=64*ClusterNum
        ld      de,0a102h+2
        add     hl,de     ; hl=0a102h+40h*ClusterNum
        ld      a,(hl)
        cp      'M'       ; selfmod code
$TypeToCompare   equ $-1   ;
        jp      nz,Scontinue
$JumpToCompare:                 ;;
        jp      $MatchFilename  ;; selfmod
Matched:
        call    restore_a000
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
$RecordLength    equ $-2 ;; selfmod
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

$OZ770Search:
        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        ld      a,e
        or      d
        jr      z,erased770
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
        pop     ix
        pop     iy
        jp      restore_a000

$MatchCFileName770:
        dec     hl
        ld      a,(hl)
        dec     hl
        or      (hl)
        jp      nz,Scontinue ;; not first part
;        inc     hl
;        inc     hl
;        inc     hl
        ld      a,($filenamelength)
        inc     a
        ld      b,a
        ld      de,36
        add     hl,de    ;; filename location
        ld      de,$findbuffer
        jp      Comparison

MatchCFileName:
        ld      de,13-2
        add     hl,de
        ld      a,(hl)
        xor     'c'.or.80h
        inc     hl
        or      (hl)
        jp      nz,Scontinue
        ld      e,5     ; d=0 still
        add     hl,de
;        inc     hl       ; now at owner_id
;        inc     hl
;        inc     hl       ; now at file_id.low
;        inc     hl       ; now at file_id.high
;        inc     hl       ; now at part_number.low
        ld      a,(hl)
        inc     hl       ; now at part_number.hight
        or      (hl)              ;; check that part number is zero
        jp      nz,Scontinue
        ld      de,11
        add     hl,de
        ld      b,(hl)            ;; filename length
        inc     b
        inc     hl                ;; now at filename
DoComparison:
        ld      de,$findbuffer
Comparison:
        ld      a,(de)
        cp      (hl)
        jp      nz,Scontinue
        inc     hl
        inc     de
        djnz    Comparison
        jp      Matched

$MatchFilename:
        ld      de,20
        add     hl,de
        ld      b,MemoTitleLength
        jp      DoComparison

;        call    strcmp
;        ld      a,l
;        or      h
;        jp      nz,Scontinue
;        jp      Matched
;
;
;        call    strcmp
;        ld      a,l
;        or      h
;        jp      nz,Scontinue
;        jp      Matched

$MatchFileID770:
        ld      de,60-2
        add     hl,de
        ld      a,(hl)
        cp      (ix)
        jp      nz,Scontinue
        inc     hl
        ld      a,(hl)
        cp      (ix+1)
        jp      nz,Scontinue
        jp      Matched


