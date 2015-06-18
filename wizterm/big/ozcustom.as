global __ozcustomisr,_ozserialgetc,_ozintwait,_ozkeyclick,__ozserbufget,__ozserbufput
global _ozexitrestoreint,KeyBufGetPos,KeyBufPutPos,KeyboardBuffer,__ozclick_setting
global __oz64hz_word
global __oz64hz_dword
global __ozrxhandshaking,__ozrxxoff

psect data
__ozrxhandshaking: defb 0
__ozrxxoff:      defb 0

psect text

PauseFirst     equ 32
PauseBetween   equ 5

BufLen EQU 256

_ozintwait:
;; waits for a keystroke, serial data, or interrupt event
        ld a,(__ozserbufget)
        ld c,a
        ld a,(__ozserbufput)
        cp c
        ret nz
        ld a,(KeyBufGetPos)
        ld c,a
        ld a,(KeyBufPutPos)
        cp c
        ret nz
        halt
        ret

__ozcustomisr:
        in      a,(46h)
        and     16
        jp      nz,_ozexitrestoreint  ;; power switch depressed - get out quickly
        in      a,(12h)
        and     128
        jp      z,_ozexitrestoreint   ;; battery switch flipped - get out rapidly
        in      a,(5)
        ld      d,a
        ld      a,0ffh
        out     (6),a

        in      a,(45h)
        and     1
        jp      z,GetOutPopAF
        in      a,(40h)
        push    hl
        ld      e,a
        ld      hl,__ozserbufput
        ld      a,(hl)
        ld      c,a
        inc     a
        dec     hl  ; hl=__ozserbufget
        cp      (hl)
        jr      z,BufferFull
        inc     hl  ; hl=__ozserbufput
        ld      (hl),a

        ld      b,0
        inc     hl  ; hl=SerialBuffer
        add     hl,bc
        ld      (hl),e

        ld      hl,__ozserbufget
        sub     (hl)      ; a=buffer size
        cp      200
        jr      c,noXOFF
        ld      a,(__ozrxhandshaking)
        or      a
        jr      z,noXOFF
        ld      a,(__ozrxxoff)
        or      a
        jr      nz,noXOFF
waittop:
        in      a,(45h)
        and     20h
        jr      z,waittop
        ld      a,19  ; XOFF
        out     (40h),a
        ld      a,1
        ld      (__ozrxxoff),a
noXOFF:
BufferFull:
        pop     hl
GetOutPopAF:
        ei
        ld      a,d      ; from port (5)
        and     32 ;+16
        jp      nz,KbdAndClock
        ret

KbdAndClock:
        ld      a,(keybusy)
        or      a
        ret     nz
        dec     a      ; a=ff
        ld      (keybusy),a
        jp      key_isr
key_isr_ret:
        xor     a
        ld      (keybusy),a
        ret

psect bss
__oz64hz_word:
__oz64hz_dword:
        defs 4
keybusy:
        defs 1
__ozserbufget:
        defs 1
__ozserbufput:
        defs 1
SerialBuffer:
        defs BufLen

; Keyboard driver adapted from software labeled:
; Source code free for non-commercial use
; Assembled program (C)2000 OZdev <http://www.ozdev.com/>
; Benjamin Green <benjamin@lh.co.nz>

keyTable:        defs 10

;; global _mask10,_mask11,_repeatcount,_repeatkey
_mask10:         defs   1
_mask11:         defs   1
_repeatcount:    defs   1
_repeatkey:      defs   2

Mask2nd   equ 01h
MaskNew   equ 02h
MaskInv   equ 04h
MaskShift equ 08h

kUpperMenu equ  8068h
kUpperEnter equ 8066h
kUpperEsc  equ  8067h
kEnter  equ    8038h
kLShift equ    0000h
kRShift equ    8836h
k2nd    equ    8137h
kEsc    equ    8058h
kMenu   equ    8032h
kNew    equ    8233h
kInv    equ    8435h
kUp     equ    8040h
kDown   equ    8041h
kRight  equ    8043h
kLeft   equ    8042h
kMain   equ    7025h
kTel    equ    70e9h
kSched  equ    70eah
kMemo   equ    70ebh
kMyProg equ    7015h
kLight  equ    803bh
kPageUp equ    8044h
kPageDown       equ    8045h

psect text
key_isr:
	push	bc
	push	de
	push	hl

        ld      hl,(__oz64hz_dword)
        ld      bc,1
        add     hl,bc
        ld      (__oz64hz_dword),hl
        ld      hl,(__oz64hz_dword+2)
        ld      bc,0
        adc     hl,bc
        ld      (__oz64hz_dword+2),hl

        ld      a,(__ozrxxoff)
        or      a
        jr      z,NoXOFFcheck
        ld      hl,__ozserbufput
        ld      a,(hl)
        dec     hl       ; hl=__ozserbufget
        sub     (hl)
        cp      150
        jr      nc,NoXOFFcheck
waittop2:
        in      a,(45h)
        and     20h
        jr      z,waittop2
        ld      a,17  ; XON
        out     (40h),a
        xor     a
        ld      (__ozrxxoff),a
NoXOFFcheck:
        ld      a,(KeyBufPutPos)
        inc     a
        cp      0ch
        jr      c,dontzero
        xor     a
dontzero:
        ld      b,a
        ld      a,(KeyBufGetPos)
        cp      b
        jp      z,KBufferFull

        xor     a
	out	(11h),a
	ld	a,2
	out	(12h),a
	ld	hl,keyTable+8
	in	a,(10h)
	ld	b,(hl)
	ld	(hl),a
	or	b
	xor	b
	and	a
        jr      z,noProgKey1

bitP0:	bit	0,a
	jr	z,bitP1
        ld      a,56
        jp      lookup
bitP1:	bit	1,a
	jr	z,bitP2
        ld      a,57
        jr      lookup
bitP2:	bit	2,a
	jr	z,bitP3
        ld      a,58
	jr	lookup
bitP3:	bit	3,a
	jr	z,bitP4
        ld      a,59
	jr	lookup
bitP4:	bit	4,a
	jr	z,bitP6
        ld      a,60
	jr	lookup
bitP6:	bit	6,a
        jr      z,noProgKey1
        ld      a,61
        jr      lookup

noProgKey1:
        ld      a,1
	out	(12h),a
        ld      hl,keyTable+9
	in	a,(10h)
	ld	b,(hl)
	ld	(hl),a
	or	b
	xor	b
	and	a
        jr      z,noProgKey2

abitP0:  bit     0,a
        jr      z,abitP3
        ld      a,62
        jp      lookup
abitP3:  bit     3,a
        jr      z,abitP4
        ld      a,63
        jr      lookup
abitP4:  bit     4,a
        jr      z,abitP5
        ld      a,64
	jr	lookup
abitP5:  bit     5,a
        jr      z,abitP6
        ld      a,65
	jr	lookup
abitP6:  bit     6,a
        jr      z,noProgKey2
        ld      a,66
lookup: ld      e,a
        xor     a
        ld      (_mask11),a
        jr      lookup2

noProgKey2:
        xor     a
	out	(12h),a
	ld	hl,keyTable
	ld	de,100h

keyscan:
        ld      a,d
	out	(11h),a
	in	a,(10h)
	ld	b,(hl)
	ld	(hl),a
	or	b
	xor	b
	and	a
	jr	nz,bit0
	inc	e
	inc	hl
	sla	d
	jr	nz,keyscan

bit0:   bit     0,a
	jr	z,bit1
	ld	a,e
        ld      l,1
        jr      rlookup
bit1:	bit	1,a
	jr	z,bit2
	ld	a,e
        add     a,8
        ld      l,2
        jr      rlookup
bit2:	bit	2,a
	jr	z,bit3
	ld	a,e
        add     a,16
        ld      l,4
        jr      rlookup
bit3:	bit	3,a
	jr	z,bit4
	ld	a,e
        add     a,24
        ld      l,8
        jr      rlookup
bit4:	bit	4,a
	jr	z,bit5
	ld	a,e
        add     a,32
        ld      l,16
        jr      rlookup
bit5:	bit	5,a
	jr	z,bit6
	ld	a,e
        add     a,40
        ld      l,32
        jr      rlookup
bit6:	bit	6,a
        jr      z,nokey
	ld	a,e
        add     a,48
        ld      l,64

rlookup:
        ld      e,a
        ld      a,l
        ld      hl,_mask10
        ld      (hl),a
        inc     hl  ; =_mask11
        ld      (hl),d

lookup2:
        xor     a
	ld	hl,keyTable
        ld      b,(hl)
        bit     6,b
	jr	z,no2nd
        ld      a,Mask2nd
no2nd:	bit	5,b
        jr      nz,shift
        ld      hl,keyTable+6
        bit     3,(hl)
        jr      z,noShift
shift:  or      MaskShift
noShift:
        ld      hl,keyTable+3
        bit     6,(hl)
        jr      z,noInv
        or      MaskInv
noInv:
        dec     hl
        bit     6,(hl)
        jr      z,noNew
        or      MaskNew
noNew:
lookupKey:
        ld      b,a
        ld      hl,keys
	ld	d,0
	add	hl,de
        add     hl,de
	ld	a,(hl)
        ld      c,a
        inc     hl
        ld      a,(hl)
        xor     b
        ld      b,a

        ld      hl,_repeatcount
        ld      (hl),PauseFirst
        inc     hl      ; =_repeatkey
        ld      (hl),c
        inc     hl
        ld      (hl),b

putinbuf:
        ld      a,(__ozclick_setting)
        or      a
        call    nz,_ozkeyclick
putinbuf_noclick:

        ld      a,(KeyBufPutPos)
        inc     a
        cp      0ch
        jr      c,dontzero2
        xor     a
dontzero2:
        ld      (KeyBufPutPos),a
        ld      e,a
        ld      d,0
        ld      hl,KeyboardBuffer
        add     hl,de
        add     hl,de
        ld      (hl),c
        inc     hl
        ld      (hl),b
KBufferFull:
iret:	ld	a,0ffh
	out	(11h),a
	out	(12h),a
	pop	hl
	pop	de
	pop	bc
        jp      key_isr_ret

clearrepeat:
        xor     a
        ld      (_mask11),a  ; hl=_mask11
        jr      iret

nokey:
        ld      hl,_mask11
        xor     a
        or      (hl)
        jr      z,iret  ; nothing to repeat
        ld      c,a

        inc     hl      ; =_repeatcount
        inc     hl      ; =_repeatkey
        ld      hl,_repeatkey
        xor     a
        ld      b,(hl)
        or      b
        jr      z,clearrepeat  ; lower shift - do not repeat
        inc     hl
        ld      a,080h
        and     (hl)
        jr      z,doRepeat  ; not any other shift key
        ld      a,b
        cp      kRShift.and.0ffh
        jr      z,clearrepeat
        cp      kNew.and.0ffh
        jr      z,clearrepeat
        cp      kInv.and.0ffh
        jr      z,clearrepeat
        cp      k2nd.and.0ffh
        jr      z,clearrepeat

doRepeat:
        ld      hl,_mask10
        ld      a,c
        out     (11h),a ; set mask
        xor     a
        out     (12h),a
        in      a,(10h)
        and     (hl)    ; (_mask10)
        inc     hl      ; =_mask11
        jr      z,clearrepeat  ; key to be repeated released
        inc     hl      ; =_repeatcount
        dec     (hl)
        ld      a,(hl)
        or      a
        jr      nz,iret ; not time yet

        ld      (hl),PauseBetween

        inc     hl      ; =_repeatkey
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        jr      putinbuf_noclick

keys:   defw     kEsc,'1','2','3','4','5','6','7'
        defw     'q','w','e','r','t','8','9','0'
        defw     0,'y','u','i','o','p',8,'.'
        defw     'g','h','j','k','l',13,kRShift,','
        defw     'a','s','d','f',kLeft,kDown,kRight,kUp
        defw     kLShift,'z','x','c','v','b','n','m'
        defw     k2nd,kMenu,kNew,kInv,' ','-',kEnter,0
        defw     kMain,kTel,kSched,kMemo,kMyProg,kLight
        defw     kUpperMenu,kUpperEsc,kUpperEnter,kPageUp,kPageDown

_ozserialgetc:
        ld      a,(__ozserbufget)
        ld      e,a
        ld      a,(__ozserbufput)
        cp      e
        jr      z,NothingInBuffer
        ld      l,e
        ld      h,0
        ld      bc,SerialBuffer
        add     hl,bc
        ld      a,(hl)
        ld      l,a
        ld      h,0
        ld      a,e
        inc     a
        ld      (__ozserbufget),a
        ret
NothingInBuffer:
        ld      hl,-1
        ret

_ozkeyclick:
        in      a,(15h)
        and     a
        ret     nz      ; bell char takes priority
        ld      a,1
        out     (19h),a
        out     (16h),a
        xor     a
keyclick:       dec     a
        jr      nz,keyclick
        out     (16h),a
dontClick:
        ret
