;       run time start off for Hi-Tech C
;               allows custom ISRs via ozsetisr()

        global _main,_exit,__Hbss,__Lbss,$reserved_fe00$,_ozexitrestoreint

$reserved_fe00$ equ 1

        psect lowpage

        DEFB 00h, 64h, 09h, 0FEh, 31h, 26h, 48h, 41h
        DEFB 30h, 31h, 32h,  0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh

start:
global _memo1

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

        push   de
        push   bc
        push   hl
        call   _memo1
        pop    hl
        pop    bc
        pop    de
        jp     restore_old


        ld     hl,0407h
        ld     a,l
        out    (3),a
        ld     a,h
        out    (4),a     ; page in second code page

        in     a,(7)
        ld     (intset),a

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
        ld     ($sp$),a
        ld     a,h
        ld     ($sp$+1),a

;; save stack to page 409h
        ld     bc,0408h
        ld     a,c
        out    (3),a
        ld     a,b
        out    (4),a

        ld     de,0
        ex     de,hl
        cp     a
        sbc    hl,de
        ld     c,l
        ld     b,h    ; bc=length of stack
        ld     a,c
        ld     (_slen),a
        ld     a,b
        ld     (_slen+1),a
      ;;  ld     bc,2000h
      ;;  ld     bc,500h
        ld     de,0a000h
        ld     hl,0
        add    hl,sp

        ldir


        di
        ld     sp,0fe11h ; 0fe00h
        ei

        ld     bc,0407h
        ld     a,c
        out    (3),a
        ld     a,b
        out    (4),a
;; stack saved
;;
;; there is free space between fe00 and ffff now ;;


        call    _main    ;call main program
__exit:
        di
        im     1
        ld     bc,0408h
        ld     a,c
        out    (3),a
        ld     a,b
        out    (4),a

        ld     hl,0
        ld     a,($sp$)
        ld     e,a
        ld     a,($sp$+1)
        ld     d,a
        push   de

        or     a
        sbc    hl,de
        ld     c,l
        ld     b,h    ; bc=length of old stack
        ld     hl,0a000h
        pop    de

        ldir
        ld     a,($sp$)
        ld     l,a
        ld     a,($sp$+1)
        ld     h,a
        ld     sp,hl

restore_old:
        ld     hl,0406h
        ld     a,l
        out    (3),a
        ld     a,h
        out    (4),a
        jp     continue2+2000h
continue2:
        pop    hl
        ld     a,l
        out    (1),a
        ld     a,h
        out    (2),a     ; restore starting page
        ei
        ret

_ozexitrestoreint:
        di
intset  equ    $+1
        ld     a,0ffh
_exit:
        out    (7),a
        ld      a,6-4
        out     (1),a
        ld      a,4
        out     (2),a
        jp      __exit

$sp$:
        defs 2

global _slen
_slen:
        defs 2

        psect text
        psect data
        psect bss
