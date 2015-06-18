;       run time start off for Hi-Tech C
;               allows custom ISRs via ozsetisr()

        global _main,_exit,__Hbss,__Lbss,_ozexitrestoreint
        global __ozfilename,_ozsetisr,__ozcustomisr, HeapTop
        global KeyBufGetPos, KeyBufPutPos, KeyboardBuffer, EnableKeyboard
        global __ozbacklight,ScrCharSet,$swapupperpages,_bdos
        global $32kintoff,$32kinton,$sp$,Model32k
        global __ozcallexitfunctions,__exit,__ozfileowner
        global $ozlcdstatus ;; ,__ozbacklight_save
        global __ozcontrast,_ozclick
        global $clearbacklighttimer
        global __ozspare1start,__ozspare1end
        global $init_unblank


        psect lowpage

                
        defw $compress_jump  ;basic token for line number is set to $compress_jump
        DEFB 09h, 0FEh, 31h, 26h, 48h, 41h , 30h, 31h, 32h

        DEFB 0Dh, 75h, 31h, 03h, 0FEh, 5Ah, 0Fh  ; lf / 30001 end 
;       DEFB 3Ah, 0FEh, 5Ah, 0Fh                 ; :end tokens  (alternative)

start:
        jr     skipname
__ozfilename:
        defm    'BFILNAMEBAS'
        defb    00
__ozfileowner:
        defw    65535
__ozbacklight:
        defb    0
;__ozbacklight_save:
;        defb    0
$ozlcdstatus:
        defw  0
__ozcontrast:
        defb   0
intset:
        defb 0ffh
$sp$:
        defw 0fe00h

swapret: defw 0

$swapupperpages:       ;; must be called with interrupts disabled
        pop     bc
        ld      ($swapupperpages_ret),bc


        ld     a,8
        out    (3),a
        ld     a,4      ; page in page 408 at 0a000h
        out    (4),a

        ld      d,0c0h
        call    $swappage     ; swap 0a000h page with 0c000h page

        ld      a,9
        out     (3),a
;        ld      a,4
;        out     (4),a   ; page in page 409 at 0a000h

        ld      d,0e0h
        call    $swappage      ; swap 0a000h page with 0e000h page

        ld      hl,0
        add     hl,sp
        ld      de,($sp$)
        ld      ($sp$),hl
        ex      de,hl
        ld      sp,hl

        jp      0
$swapupperpages_ret equ $-2

$swappage:   ;; must be called with interrupts disabled!
        ;; swaps pages at a000 and at d*256

        pop     bc
        ld      ($swappage_ret),bc

        ld      hl,0
        add     hl,sp
        ld      ($saved_sp),hl

        ld      hl,0a000h
        ld      sp,hl
        ld      e,l
        ex      de,hl
        ld      a,h
        add     a,20h      ; end marker
top:
        pop bc   ;10
        ld e,(hl)  ;7
        ld (hl),c  ;7
        inc l       ;4
        ld d,(hl)  ;7
        ld (hl),b  ;7
        inc l        ;4
        push de  ;11
        pop de    ;10    ;SP=SP+2
        pop bc    ;10
        ld e,(hl)
        ld (hl),c
        inc l
        ld d,(hl)
        ld (hl),b
        inc l
        push de
        pop de
        jp nz,top  ;10
        inc h
        cp h
        jp nz,top

        ld      hl,0000   ;;
$saved_sp equ $-2         ;; self-mod
        ld      sp,hl
        ld      a,7
        out     (3),a
;;        ld      a,4
;;        out     (4),a     ; page in second code page
        jp      0
$swappage_ret equ $-2



__ozspare1start:
skipname:
        in     a,(1)
        push   af
        in     a,(2)
        push   af        ; save starting 8000 page
;;        ld     hl,0406h-4
;;        ld     a,l
        ld     a,2
        out    (1),a
;;        ld     a,h
        ld     a,4
        out    (2),a     ; page first code page into 8000h
        jp     continue
continue:

        ld      a,(0c068h)  ; backlight state
        ld      (__ozbacklight),a

;        ld      a,(0c003h)
;        ld      (__ozbacklight_save),a

        ld      hl,(_$ozlcdstatus)
        ld      ($ozlcdstatus),hl

        ld      a,(___ozcontrast)
        ld      (__ozcontrast),a

        di

        in     a,(7)
        ld     (intset),a
        ld     a,0ffh-(16+128+1)
        out    (7),a

        call   $swapupperpages
        jp     _unpacked       ;modified by ozpack to jump to unpacker.
$compress_jump equ $-2h        
        
;; now our code is paged in as follows:
;;  8000h: first code page, originally in page 406h
;;  a000h: second code page, originally in page 407h
;;  c000h: third code page, originally in page 408h
;;  e000h: fourth code page, originally in page 409h
;;  pages 408h and 409h store the initial contents of c000h-ffffh
;; sp points to 0fe00h
        



;ozpack will start compressing file from here onwards.

_unpacked:

; clear BSS segment (lifted from Hi-Tech C init code, but modified)
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

        ld      hl,KeyBufGetPos
        ld      (hl),0
        ld      de,KeyBufGetPos+1
        ld      bc,26-1
        ldir

;        ld      a,1
;        out     (19h),a
;        ld      a,10
;        out     (17h),a
;        xor     a
;        out     (18h),a
;        ld      a,2
;        out     (16h),a         ;; enable key click

        ld      hl,__ozcustomisr
        push    hl
        call    _ozsetisr       ;; install our ISR
        pop     bc
        ld      a,l
        or      h
        jr      nz,__exit

        ld      hl,1
        push    hl
        call    _ozclick
        pop     hl

        ld      hl,argv
        push    hl
        ld      hl,1
        push    hl
        call    _main    ;call main program
__ozspare1end:
        pop     hl
        pop     hl
_ozexitrestoreint:
_exit:
        ld      a,7
        out     (3),a
        ld      a,4
        out     (4),a   ;; page in proper second page
        call    __ozcallexitfunctions
__exit:
        call    ret_only
$init_unblank equ $-2
;;;;        call    _ozslow

        di

        ld     a,(intset)
;;;;        ld     a,0ffh-80h-1
        out    (7),a
        im     1

        call    $swapupperpages

        call    $clearbacklighttimer

;; restore a bunch of stuff
        ld      a,(__ozbacklight)
        ld      (0c068h),a
        ld      hl,($ozlcdstatus)
        ld      (_$ozlcdstatus),hl ;; restore right LCD Status
        ld      a,(__ozcontrast)
        ld      (___ozcontrast),a
        ld      hl,KeyBufGetPos ;; put keyboard buffer in standard buffer
        ld      de,0c031h
        ld      bc,24
        ldir

global $wipe_hook
$wipe_hook:
        nop
        nop
        nop


;        ld     hl,0406h
;        ld     c,4
;        out    (c),h
;        dec    c
;        out    (c),l
;        dec    c
        ld      a,6
        out     (3),a
        ld      a,4
        out     (4),a

        jp     continue2+2000h
continue2:
;        pop    hl
;        out    (c),h
;        dec    c
;        out    (c),l     ; restore starting page

        pop    af
        out    (2),a
        pop    af
        out    (1),a

        xor    a
        out    (22h),a   ; restore default display page
        ei

_bdos:                ;; dummy routines
        ret

$32kintoff:
        di
        im      1
ret_only:
        ret

$32kinton:
        ld      a,0feh
        ld      i,a
        im      2
        ei
        ret

$clearbacklighttimer:
        ld      hl,0
        ld      (0c00dh),hl
        ld      hl,($ozlcdstatus)
        ld      (_$ozlcdstatus),hl
        ret

KeyBufGetPos:   defb 0
KeyBufPutPos:   defb 0
KeyboardBuffer: ;     123456789012345678901234
                defm '(c)Pruss,Green,&c v0.101'
        psect midpage
        psect text
        psect data
ScrCharSet:     defb 1
argv:   defw __ozfilename
        psect bss
EnableKeyboard: defs 1
HeapTop EQU 0f980h
_$ozlcdstatus equ 0c024h
___ozcontrast equ 0c026h
Model32k EQU 1

