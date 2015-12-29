global _ozwriteauxmem0,_ozreadauxmem0
global $aux_pagein,$aux_pageout

;;
;; These routines only work if source/dest are in psect beforebss
;;

psect beforebss

_ozwriteauxmem0:
;; void ozwriteauxmem(unsigned address,void *data,unsigned length);

        exx
        pop     bc
        exx
        pop     de  ;; address
        ld      a,d
        add     a,080h
        ld      d,a
        pop     hl  ;; data
        pop     bc  ;; length
        push    bc
        push    hl
        push    de
        exx
        push    bc
        exx

copy:
        ld      a,b
        or      c
        ret     z

        call    $aux_pagein
        ldir
        jp      $aux_pageout

_ozreadauxmem0:
;; void ozreadauxmem(unsigned address,void *data,unsigned length);
	exx
	pop	bc
	exx
	pop	hl  ;; address
	ld	a,h
	add	a,80h
	ld	h,a
	pop	de  ;; data
	pop	bc  ;; length
	push	bc
	push	de
	push	hl
	exx
	push	bc
	exx

;;
;; hl=source address
;; de=destination address
;; bc=length
;;
        jr      copy
