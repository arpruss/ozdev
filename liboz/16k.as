;       run time start off for Hi-Tech C
;               allows custom ISRs via ozsetisr()

        global _main,_exit,__Hbss,__Lbss,_ozexitrestoreint
        global __ozfilename,KeyBufGetPos,KeyBufPutPos,KeyboardBuffer
        global __ozbacklight,__ozbacklight_save
        global EnableKeyboard,HeapTop,ScrCharSet
        global $swapupperpages,$32kintoff,$32kinton,_bdos,Model32k,$sp$
        global __ozcallexitfunctions,__exit,__ozfileowner
        global __ozinitialsp,AlarmSet,$ozlcdstatus
        global __ozcontrast,$clearbacklighttimer
        global __ozspare1start,__ozspare1end
        global $init_unblank

        psect lowpage

        DEFB 00h, 64h, 09h, 0FEh, 31h, 26h, 48h, 41h
        DEFB 30h, 31h, 32h,  0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh

start:
        jr     skipname
__ozfilename:
        defm    'BFILNAMEBAS'
        defb    00
__ozfileowner:
        defw    65535
__ozspare1start:
skipname:
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
        ld     hl,2
        add    hl,sp
        ld     (__ozinitialsp),hl

        ld     hl,0407h
        ld     a,l
        out    (3),a
        ld     a,h
        out    (4),a     ; page in second code page

        in     a,(7)
        ld     (intset),a

;        ld     a,(__ozbacklight_save)
;        ld     (saved_backlight),a

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
        ld     bc,0409h
        ld     a,c
        out    (3),a
        ld     a,b
        out    (4),a
        ld     de,0
        ex     de,hl
        or     a
        sbc    hl,de
        ld     c,l
        ld     b,h    ; bc=length of stack
        ld     de,0a000h
        ld     hl,0
        add    hl,sp
        di
        ldir
        ld     sp,0fe00h
        ei
        ld     bc,0407h
        ld     a,c
        out    (3),a
        ld     a,b
        out    (4),a
;; stack saved
;;
;; there is free space between fe00 and ffff now ;;

        ld      hl,argv
        push    hl
        ld      hl,1
        push    hl
__ozspare1end:
        call    _main    ;call main program
        pop     hl
        pop     hl
_exit:
        ld      a,7
        out     (3),a
        ld      a,4
        out     (4),a    ;; page in the right page
        call    __ozcallexitfunctions
__exit:
        call    ret_only
$init_unblank equ $-2
;;;        call    _ozslow
        call    $clearbacklighttimer
        di
        im     1
        ld     bc,0409h
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

;        ld     a,(saved_backlight)
;        ld     (__ozbacklight_save),a

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

        ld     a,0
        out    (22h),a   ; restore default display page

        ei
_bdos:              ;;
$32kintoff:         ;;
$32kinton:          ;;
$swapupperpages:    ;; dummy routines
ret_only:
        ret

_ozexitrestoreint:
        di
intset  equ    $+1
        ld     a,0ffh
        out    (7),a
        jp      _exit

$clearbacklighttimer:
        ld      hl,0
        ld      (0c00dh),hl
        ret


__ozinitialsp:
        defw 0

$sp$:
        defw 0

;saved_backlight:
;        defb 0

AlarmSet:
        defb 0

        psect midpage
        psect text
        psect data
argv:   defw __ozfilename
        psect bss

KeyBufGetPos   EQU 0c031h
KeyBufPutPos   EQU 0c032h
KeyboardBuffer EQU 0c033h
EnableKeyboard EQU 0c00fh
HeapTop        EQU 0bfffh
__ozbacklight  EQU 0c068h
__ozbacklight_save EQU 0c003h
ScrCharSet equ 0c066h
$ozlcdstatus equ 0c024h
__ozcontrast  equ 0c026h
Model32k       EQU 0
