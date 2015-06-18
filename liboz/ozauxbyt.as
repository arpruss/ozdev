global _ozwriteauxbyte,_ozreadauxbyte,__ozauxstart,$aux_pagein,$aux_pageout

psect beforebss
_ozwriteauxbyte:
;; void ozwriteauxbyte(unsigned address,byte value);

;; guaranteed not to modify arguments on stack

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
        jp      $aux_pageout

_ozreadauxbyte:
;; byte ozreadauxbyte(unsigned address);

;; guaranteed not to modify arguments on stack

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
        ld      l,(hl)
        ld      h,0
        jp      $aux_pageout

