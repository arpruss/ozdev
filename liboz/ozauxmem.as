global _ozwriteauxmem,_ozreadauxmem,__ozauxstart
global $aux_pagein0,$aux_pageout,$auxbuffer

psect beforebss

_ozwriteauxmem:
;; void ozwriteauxmem(unsigned address,void *data,unsigned length);
	exx
	pop	bc
	exx
	pop	de  ;; address
	pop	hl  ;; data
	pop	bc  ;; length
	push	bc
	push	hl
	push	de
	exx
	push	bc
	exx

	ex	de,hl		   ;; de=data, hl=address
	push	bc
	ld	bc,(__ozauxstart)
	add	hl,bc		   ;; adjust data
	pop	bc
	ld	a,h
	cp	40h
	ret	nc		 ;; out of bounds
	push	hl
	add	hl,bc
	dec	hl
	ld	a,h
	pop	hl
	cp	40h
	ret	nc		;; out of bounds
	ld	a,h
	add	a,80h
	ld	h,a
	ex	de,hl		;; de=adjusted address, hl=data


;;
;; de=destination address after paging in
;; hl=source address
;; bc=length
;;
	ld	a,b
	or	a
	jr	z,LastPart
CopyStart:
	push	bc
	push	de
	ld	de,$auxbuffer
	ld	bc,100h
	ldir
	pop	de
	push	hl
	ld	hl,$auxbuffer
	ld	bc,100h
	call	$aux_pagein0
	ldir
	call	$aux_pageout
	pop	hl
	pop	bc
	djnz	CopyStart
LastPart:
	ld	a,c
	or	a
	ret	z
;;	  ld	  a,c
	push	de
	ld	de,$auxbuffer
	ldir
	pop	de
	ld	c,a
	ld	hl,$auxbuffer
	call	$aux_pagein0
	ldir
	jp	$aux_pageout

_ozreadauxmem:
;; void ozreadauxmem(unsigned address,void *data,unsigned length);
	exx
	pop	bc
	exx
	pop	hl  ;; address
	ld	bc,(__ozauxstart)
	add	hl,bc		   ;; adjust address
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
	ld	a,b
	or	a
	jr	z,LastPart2
CopyStart2:
	push	bc
	push	de
	ld	de,$auxbuffer
	ld	bc,100h
	call	$aux_pagein0
	ldir
	call	$aux_pageout
	pop	de
	push	hl
	ld	hl,$auxbuffer
	ld	bc,100h
	ldir
	pop	hl
	pop	bc
	djnz	CopyStart2
LastPart2:
	ld	a,c
	or	a
	ret	z
	push	bc
	push	de
	ld	de,$auxbuffer
	call	$aux_pagein0
	ldir
	call	$aux_pageout
	pop	de
	pop	bc
	ld	hl,$auxbuffer
	ldir
	ret

