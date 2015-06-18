global $aux_pagein,$aux_pageout,__ozauxstart,_ozcrc16auxmem

psect beforebss

empty:
	ld h,a	;; a = 0
	ld l,a
	ret

_ozcrc16auxmem:
;; unsigned ozcrc16auxmem(unsigned start,unsigned length);
  
	pop	de
  pop     hl ;; start
  pop     bc ;; length
	push	bc
	push	hl
	push	de  

	ld	a,c
	or	b
	jr	z,empty

	;ld	de,8000h
	;add	hl,de
  set 7,h
	ld	de,(__ozauxstart)
	add	hl,de

  call    $aux_pagein

  exx
  ld      de,0ffffh
  ld      h,0Fh
  exx

;; hl=data, bc=length, 'de=crc

crcloop:

  ld  a,(hl)
  exx
  xor e
  ld b,a
  and h ; h = 0Fh
  rla
  rla
  rla
  rla
  xor b
  ld b,a
  ld e,d
  ld d,a
  xor a
  rl b
  rla
  rl b
  rla
  rl b
  rla
  ld c,a
  xor d
  ld d,a
  ld a,e
  xor b
  ld e,a
  ld a,c
  rl b
  rla
  xor e
  ld e,a 
  exx

  cpi             ;decr bc , incr hl 
  jp pe,crcloop

  cpl           ;'e' from CRC still in 'a'
  ld h,a
  exx
  ld a,d
  exx
  cpl
  ld l,a
  jp      $aux_pageout


