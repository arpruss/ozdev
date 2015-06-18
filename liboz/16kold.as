;       Run time start off for Hi-Tech C

        global _main,$gint,_exit,__Hbss,__Lbss

        psect lowpage

        DEFB 00h, 64h, 09h, 0FEh, 31h, 26h, 48h, 41h
        DEFB 30h, 31h, 32h,  0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh

start:
        in     a,(1)
        ld     l,a
        in     a,(2)
        ld     h,a       ; save starting 8000 page
        push   hl
        ld     hl,0406h-4
        ld     a,l
        out    (1),a
        ld     a,h
        out    (2),a     ; page first code page into 8000h
        jp     continue
continue:
        ld     hl,0407h
        ld     a,l
        out    (3),a
        ld     a,h
        out    (4),a     ; page in second code page


; clear BSS segment (lifted from Hi-Tech C init code)
        ld      de,__Lbss       ;Start of BSS segment
	or	a		;clear carry
	ld	hl,__Hbss
	sbc	hl,de		;size of uninitialized data area
        jr      z,EmptyBSS
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
        ldir                    ;clear memory
EmptyBSS:

        ld     hl,0
        add    hl,sp
        ld     a,l
        ld     ($spLow$+1),a
        ld     a,h
        ld     ($spHigh$+1),a

        call    _main    ;call main program
$getout$:
        ld     hl,0406h
        ld     a,l
        out    (3),a
        ld     a,h
        out    (4),a
        jp     continue2+2000h
continue2:
        pop     hl
        ld     a,l
        out    (1),a
        ld     a,h
        out    (2),a     ; restore starting page
        ret


_exit:
$spLow$:
        ld     l,0
$spHigh$:
        ld     h,0
        ld     sp,hl
        jp     $getout$


$spbeforemain$:
        defw 0

;
;*****************************************************
;						     *
;	runtime	library	for small C compiler	     *
;						     *
;	c.s - runtime routine for basic C code	     *
;						     *
;		Ron Cain			     *
;						     *
;*****************************************************

        psect   text

; fetch int from (HL)
$gint:
        ld      a,(hl)
	inc	hl
	ld	h,(hl)
	ld	l,a
	ret

        psect   bss
        psect   data
