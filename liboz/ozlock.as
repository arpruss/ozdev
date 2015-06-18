global _ozdetectmodel,$ozlock

psect lowpage

$ozlock:
        di
        call    __ozlockos_atstart
        push    hl
        ld      hl,__ozlockos_atstart
        ld      (hl),0
        ld      de,__ozlockos_atstart+1
        ld      bc,__ozlockos_end-__ozlockos_atstart-1
        ldir
        pop     hl
        ret

__ozlockos_atstart:
; byte ozlockos(void)
; returns 0 if successful, otherwise contents of status register
; see flash data sheet for meaning of status register bits
; Registers modified:  AF, C, HL
        call    _ozdetectmodel
        bit     2,l
        jr      z,JustDo

;; multi-lingual models: clear pages 0-70
        ld      a,70h
        ld      ($clearto),a

JustDo:
	in		a, (04h)
	push	af
	in		a, (03h)
	push	af						; save starting page
	ld		hl, 0A000h
	ld		c, -8					; start at block 0
1:	ld		a, c
	add	a, 08h				; move to next block
	cp		48h					; thru the OS yet?
$clearto equ $-1
	jp		z, 3f					; yep, get out
	ld		c, a
	out	(03h), a
	xor	a
	out	(04h), a
	ld		(hl), 60h			; SET BLOCK LOCK-BIT
	ld		(hl), 01h			; CONFIRM
2:	bit	7, (hl)				; wait for ready
	jp		z, 2b
	bit	4, (hl)				; was there an error?
	jp		z, 1b					; nope, keep going
3:	res	7, (hl)				; clear WSM status bit
	ld		c, (hl)				; read status register
	ld		(hl), 50h			; CLEAR STATUS REGISTER
	ld		(hl), 0FFh			; READ ARRAY / RESET
	pop	af						; restore starting page
	out	(03h), a
	pop	af
	out	(04h), a
        ld      a,c                                    ; return status register
        and     16
        ld      l,a
	ret	
__ozlockos_end:
