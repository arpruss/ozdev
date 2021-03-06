global $aux_pagein,$aux_pageout
global _ozwriteauxword2,_ozreadauxword2,_copymv,_scalehistory,_gettime
global _distance

psect beforebss

_ozwriteauxword2:
;; void ozwriteauxword2(unsigned address,unsigned value);
        call    $aux_pagein
        pop     bc
        pop     hl
        pop     de
        push    de
        push    hl
        push    bc
        add     hl,hl
        ld      a,h
        add     a,080h
        ld      h,a
        ld      (hl),e
        inc     hl
        ld      (hl),d
        jp      $aux_pageout

_ozreadauxword2:
;; unsigned ozreadauxword2(unsigned address);
        call    $aux_pagein
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

psect text
_copymv:
;; copymv(dest,src);
        pop     bc
        pop     de
        pop     hl
        push    hl
        push    de
        push    bc
        ld      bc,6
        ldir
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


