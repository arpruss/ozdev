global $aux_pagein,$aux_pageout
global _addhistory,_ozreadauxword2,_copymv,_scalehistory,_gettime
global _distance,_taxicab,_clearhistory

psect beforebss

_addhistory:
;; uchar addhistory(unsigned address,unsigned value);
;; Returns 1 on overflow
        ld      a,5
        out     (3),a
        xor     a
        out     (1),a
;;        call    $aux_pagein
        pop     de
        pop     hl ; address
        pop     bc ; value
        push    bc
        push    hl
        push    de
        add     hl,hl  ; address*2
        ld      a,h
        add     a,080h
        ld      h,a

        ld      e,(hl)
        inc     hl
        ld      d,(hl)
        dec     hl
        ex      de,hl
        add     hl,bc
        ex      de,hl
        ld      (hl),e
        inc     hl
        ld      (hl),d
        ld      l,0
        jp      nc,$aux_pageout
        inc     l
        jp      $aux_pageout

_ozreadauxword2:
;; unsigned ozreadauxword2(unsigned address);
        ld      a,5
        out     (3),a
        xor     a
        out     (1),a
;;        call    $aux_pagein
        pop     bc
        pop     hl
        push    hl
        push    bc
        add     hl,hl
        ld      a,h
        add     a,080h
        ld      h,a
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        jp      $aux_pageout

_scalehistory:
        call    $aux_pagein
        ld      hl,0a000h-1
        ld      e,0
        ld      a,07Fh
top:
        ld      d,(hl)
        ld      (hl),e
        dec     hl
        ld      (hl),d
        dec     hl
        cp      h
        jp      c,top
        jp      $aux_pageout

_clearhistory:
        call    $aux_pagein
        ld      hl,08000h
        ld      (hl),l
        ld      d,h
        ld      e,1
        ld      bc,8191
        ldir
        jp      $aux_pageout

psect text
_copymv:
;; copymv(dest,src);
        pop     bc
        pop     de ;dest
        pop     hl ;src
        push    hl
        push    de
        push    bc

        ldi
        ldi
        ldi

        ldi
        ldi
        ldi

        ret

global __oz64hz_dword
_gettime:
        ld      de,(__oz64hz_dword)
        ld      a,(__oz64hz_dword+2)

        sll     e
        rl     d
        rla

        sll     e
        rl     d
        rla

        ld      l,d
        ld      h,a
        ret

_distance:
        pop     bc
        pop     hl
        pop     de
        push    de
        push    hl
        push    bc

        ld      a,l
        and     7
        ld      h,a
        ld      a,e
        and     7
        sub     h
        jp      p,DontNeg
        neg
DontNeg:
        ld      d,a  ;; h=difference in columns

        ld      a,l
        srl     a
        srl     a
        srl     a

        ld      h,a
        ld      a,e
        srl     a
        srl     a
        srl     a
        sub     h
        jp      p,DontNeg2
        neg
DontNeg2:
        cp      d
        jr      c,RetD
        ld      l,a
        ld      h,0
        ret
RetD:
        ld      l,d
        ld      h,0
        ret

_taxicab:
        pop     bc
        pop     hl
        pop     de
        push    de
        push    hl
        push    bc

        ld      a,l
        and     7
        ld      h,a
        ld      a,e
        and     7
        sub     h
        jp      p,zDontNeg
        neg
zDontNeg:
        ld      d,a  ;; h=difference in columns

        ld      a,l
        srl     a
        srl     a
        srl     a

        ld      h,a
        ld      a,e
        srl     a
        srl     a
        srl     a
        sub     h
        jp      p,zDontNeg2
        neg
zDontNeg2:
        add     a,d
        ld      l,a
        ld      h,0
        ret

;global _toswap1,_toswap2,_doswap
;_doswap:
;        ld      hl,(_toswap1)
;        ld      de,(_toswap2)
;        ld      b,6
;swap_top:
;        ld      c,(hl)
;        ld      a,(de)
;        ld      (hl),a
;        ld      a,c
;        ld      (de),a
;        inc     hl
;        inc     de
;        djnz    swap_top
;        ret
