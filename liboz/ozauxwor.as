global __ozauxstart,$aux_pagein,$aux_pageout
global _ozwriteauxword,_ozreadauxword

psect beforebss

_ozwriteauxword:
;; void ozwriteauxword(unsigned address,unsigned value);
        call    $aux_pagein
        pop     bc
        pop     hl
        pop     de
        push    de
        push    hl
        push    bc
        ld      bc,(__ozauxstart)
        add     hl,bc
        ld      a,h
        cp      40h
        jp      nc,$aux_pageout ;; out of bounds
        add     a,080h
        ld      h,a
        ld      (hl),e
        inc     hl
        ld      a,h
        cp      0c0h
        jp      nc,$aux_pageout ;; out of bounds
        ld      (hl),d
        jp      $aux_pageout

_ozreadauxword:
;; unsigned ozreadauxword(unsigned address);
        call    $aux_pagein
        pop     bc
        pop     hl
        push    hl
        push    bc
        ld      bc,(__ozauxstart)
        add     hl,bc
        ld      a,h
        add     a,080h
        ld      h,a
        ld      a,(hl)
        inc     hl
        ld      h,(hl)
        ld      l,a
        jp      $aux_pageout

