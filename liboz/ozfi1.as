*include macros.inc

global $pages,$cdir_pages,_ozinitfs,_ozinclocation,_ozloadcluster
global __ozcdirpages,fs_init
global __ozclusterbuffer,__ozmodel,_ozdetectmodel,restore_a000

psect lowpage
fs_init:
        defb    0
$pages:
*include 128zeros.inc
        defw    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0  ;160 bytes
        defw    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0  ;192 bytes
        defw    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0  ;224 bytes
        defw    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0    ;254 bytes
        defb    0

__ozcdirpages:
$cdir_pages:
        defw    0,0,0,0, 0,0,0,0,0 ; assume cdir <= 8 blocks

_ozinitfs:
        ld      hl,$pages
        ld      (hl),0
        ld      de,$pages+1
        ld      bc,254
        ldir

        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel

        ld      bc,88h
ScanHeads:
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ld      hl,0a000h
        ld      a,(hl)
        or      a
        jr      z,Skip8
        cp      7fh
        jr      nz,NextPage
        inc     hl
        ld      a,(hl)
        cp      1
        jr      nz,CheckCDir
        inc     hl
        inc     hl
        ld      a,(hl)
        cp      -1
        jr      nz,NextPage
        dec     hl
        ld      e,(hl)
        ld      a,e
        or      a
        jr      z,NextPage
        ld      d,0
        ld      hl,$pages-1
        add     hl,de
        ex      de,hl  ;; store address in de for now
        push    bc
        ld      l,c
        ld      h,b
        ld      bc,80h
        or      a
        sbc     hl,bc
        pop     bc
        ex      de,hl  ;; hl=address for page, and de=adjusted page number
        ld      (hl),e
        jr      NextPage
CheckCDir:
        cp      2
        jr      nz,NextPage
        inc     hl
        inc     hl
        ld      a,(hl)
        cp      -1
        jr      nz,NextPage
        dec     hl
        ld      e,(hl)
        ld      a,e
        or      a
        jr      z,NextPage
        ld      d,0
        ld      hl,$cdir_pages-2
        add     hl,de
        add     hl,de
        ld      (hl),c
        inc     hl
        ld      (hl),b
Skip8:
        inc     bc
        inc     bc
        inc     bc
        inc     bc
        inc     bc
        inc     bc
        inc     bc  ; bc+=7
NextPage:
        inc     bc
        ld      a,b
        cp      1
        jr      c,ScanHeads
        ld      a,c
        cp      80h
        jr      c,ScanHeads
        ld      a,1
        ld      (fs_init),a
        jp      restore_a000

_ozinclocation:
        ld      hl,2
        add     hl,sp
        ld      e,(hl)
        inc     hl
        ld      c,(hl)
        push    de
        push    bc

        call    GetAddressPage
        call    GetAddressOffset

        ld      a,(hl)
        or      a
        pop     bc
        pop     de
        jp      nz,NotStandardIncrease

        ld      l,e
        ld      h,0
        ld      de,LogicalToPhysicalCluster-1
        add     hl,de
        ld      l,(hl)
        ld      h,0
        ld      de,PhysicalClusterToLogical-1 + 1 ;; incremented
        add     hl,de
        ld      l,(hl)
        ld      h,c
        jp      restore_a000
NotStandardIncrease:
        inc     hl
        ld      h,(hl)
        ld      l,a
        and     h
        inc     a
        jp      nz,restore_a000 ;; if the start of the cluster is FFFF, have EOF
        ld      hl,-1
        jp      restore_a000

_ozloadcluster:
        ld      hl,2
        add     hl,sp
        ld      e,(hl)
        inc     hl
        ld      c,(hl)

        call    GetAddressPage
        call    GetAddressOffset

        ld      de,clusterbuffer
        ld      bc,64

        ldir
        ld      hl,clusterbuffer

        jp      restore_a000

GetAddressPage:
;; ld d=0 and change page to logical page in b
        ld      d,0
        ld      b,d
        ld      hl,$pages-1
        add     hl,bc

        ld      a,(hl)
        add     a,80h
        out     (3),a
        ld      a,d     ;; recall that d=0
        adc     a,d       ;; again, recall that d=0
        out     (4),a

;        ld      l,(hl)
;        ld      h,d
;        ld      bc,80h
;        add     hl,bc
;        ld      a,l
;        out     (3),a
;        ld      a,h
;        out     (4),a

        ret

GetAddressOffset:
;; continue after GetAddressPage to get the offset of the beginning of
;; the file at logical page b and logical ClusterNum e, assuming d=0.

        ld      hl,LogicalToPhysicalCluster - 1   ; translate Logical to Physical cluster number
        add     hl,de
        ld      h,(hl)
        ld      l,d   ; d=0        ;; hl=PhysicalClusterNumber

        or      a     ; clear carry
        rr      h
        rr      l
        rr      h
        rr      l     ; hl=40h*ClusterNum

        ld      de,0a100h
        add     hl,de     ; hl=0a100h+40h*ClusterNum
        ret

__ozclusterbuffer:
clusterbuffer:
          ;   12345678901234567890123456789012
        defm 'liboz file i/o routines are (c) '
        defm '2001-2002 Alexander R. Pruss....'
        defb 00  ; null-terminator

