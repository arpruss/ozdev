FLASH_RATE equ 16

global __ozcustomisr,_ozintwait,_ozkeyclick
global _exit,__exit,KeyBufGetPos,KeyBufPutPos,KeyboardBuffer,__ozclick_setting
global __oz64hz_word
global __oz64hz_dword
global __oz4sec_word
global __ozkeyrepeatspeed,__ozkeyrepeatdelay
global __ozdisablepowerkey
global $serial_check_hook,$serial_hook,$rxxoff_hook


psect bss
__ozdisablepowerkey: defs 1

psect text
PauseFirst     equ 32
PauseBetween   equ 5


_ozintwait:
;; waits for a keystroke, serial data, or interrupt event
        jp NoSerialCheck
$serial_check_hook equ $-2
NoSerialCheck:
        ld a,(KeyBufGetPos)
        ld c,a
        ld a,(KeyBufPutPos)
        cp c
        ret nz
        halt
        ret

PowerOff:
        ld      a,(__ozdisablepowerkey)
        or      a
        jp      nz,NotPowerOff
        inc     a
        ld      (__ozdisablepowerkey),a

        ld      hl,kPower
        ld      (KeyboardBuffer),hl
        ld      a,0bh
        ld      (KeyBufGetPos),a
        xor     a
        ld      (KeyBufPutPos),a
        jp      _exit

__ozcustomisr:
        in      a,(5)
        ld      d,a
        ld      a,0ffh
        out     (6),a
        in      a,(46h)
        and     16
        jp      nz,PowerOff  ;; power switch depressed
NotPowerOff:
        in      a,(12h)
        and     128
        jp      z,__exit   ;; battery switch flipped - get out rapidly

        jp      GetOutPopAF
$serial_hook    equ $-2

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
        defs 1
__oz4sec_word:
        defs 1
        defs 2
keybusy:
        defs 1

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
_mask12:         defs   1

Mask2nd   equ 01h
MaskNew   equ 02h
MaskInv   equ 04h
MaskShift equ 08h

kPower     equ  803Ah
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

        call    FlashCursor

        jp      noxoff
$rxxoff_hook    equ $-2
noxoff:

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
        ld      (_mask12),a
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
        ld      l,1
        jp      lookup
bitP1:	bit	1,a
	jr	z,bitP2
        ld      a,57
        ld      l,2
        jr      lookup
bitP2:	bit	2,a
	jr	z,bitP3
        ld      a,58
        ld      l,4
	jr	lookup
bitP3:	bit	3,a
	jr	z,bitP4
        ld      a,59
        ld      l,8
	jr	lookup
bitP4:	bit	4,a
	jr	z,bitP6
        ld      a,60
        ld      l,16
	jr	lookup
bitP6:	bit	6,a
        jr      z,noProgKey1
        ld      a,61
        ld      l,64
        jr      lookup

noProgKey1:
        ld      a,1
	out	(12h),a
        ld      (_mask12),a
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
        ld      l,1
        jp      lookup
abitP3:  bit     3,a
        jr      z,abitP4
        ld      a,63
        ld      l,8
        jr      lookup
abitP4:  bit     4,a
        jr      z,abitP5
        ld      a,64
        ld      l,16
	jr	lookup
abitP5:  bit     5,a
        jr      z,abitP6
        ld      a,65
        ld      l,32
	jr	lookup
abitP6:  bit     6,a
        jr      z,noProgKey2
        ld      a,66
        ld      l,64
lookup: ld      e,a
        xor     a
        ld      (_mask11),a
        ld      a,l
        ld      (_mask10),a
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
        jp      z,nokey
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
        xor     a
        ld      (_mask12),a

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
__ozkeyrepeatdelay equ $-1
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
        ld      hl,_mask11
        ld      (hl),a  ; hl=_mask11
        inc     hl
        ld      (hl),a  ; hl=_mask12
        jr      iret

nokey:
        ld      a,(_mask12)
        ld      hl,_mask11
        ld      c,(hl)  ; c=(_mask11)
        or      c
        jr      z,iret  ; nothing to repeat

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
        ld      a,(_mask12)
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
__ozkeyrepeatspeed equ $-1

        inc     hl      ; =_repeatkey
        ld      c,(hl)
        inc     hl
        ld      b,(hl)
        jp      putinbuf_noclick

keys:   defw     kEsc,'1','2','3','4','5','6','7'
        defw     'q','w','e','r','t','8','9','0'
        defw     0,'y','u','i','o','p',8,'.'
        defw     'g','h','j','k','l',13,kRShift,','
        defw     'a','s','d','f',kLeft,kDown,kRight,kUp
        defw     kLShift,'z','x','c','v','b','n','m'
        defw     k2nd,kMenu,kNew,kInv,' ','-',kEnter,0
        defw     kMain,kTel,kSched,kMemo,kMyProg,kLight
        defw     kUpperMenu,kUpperEsc,kUpperEnter,kPageUp,kPageDown


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

psect lowpage
global _cursor_active,_cursor_count,_cursor_pos,_cursor_byte1,_cursor_byte2
global _doxorcursor,_cursor_state,_cursor_height
_cursor_state: defb 0
_cursor_active: defb 0
_cursor_count: defb FLASH_RATE
_cursor_pos: defw 0a000h
_cursor_height: defb 0
cursor_storage: defs 2*15

FlashCursor:
        ld      a,(_cursor_active)
        or      a
        ret     z
        ld      a,(_cursor_count)
        dec     a
        jr      z,CursorTime
        ld      (_cursor_count),a
        ret
CursorTime:
        ld      a,FLASH_RATE
        ld      (_cursor_count),a
        ld      a,(_cursor_state)
        xor     1
        ld      (_cursor_state),a

_doxorcursor:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

        push    bc

        ld      hl,(_cursor_pos)

        ld      a,(_cursor_state)
        or      a

        ld      a,(_cursor_height)
        ld      bc,29
        ld      de,cursor_storage

        ex      af,af'
        push    af ;; push original af'
        ex      af,af'

        jr      z,EraseCursor  ;; if _cursor_state=0


CTop:

        ex      af,af'  ;; use a'
        ld      a,(hl)
        ld      (de),a
        or      0
_cursor_byte1 equ $-1
        ld      (hl),a
        inc     hl
        inc     de
        ld      a,(hl)
        ld      (de),a
        or      0
_cursor_byte2 equ $-1
        ld      (hl),a
        inc     de

        add     hl,bc

        ex      af,af'
        dec     a

        jr      nz,CTop


CursorCleanup:
        ex      af,af' ;; use a' again
        pop     af
        ex      af,af' ;; restore
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

EraseCursor:
        ex      de,hl  ;; de=screen,hl=cursor_storage
ECTop:
        ex      af,af'
        ld      a,(hl)
        ld      (de),a
        inc     hl
        inc     de
        ld      a,(hl)
        ld      (de),a
        ex      af,af'
        inc     hl
        ex      de,hl  ;
        add     hl,bc  ;  add de,29
        ex      de,hl  ;
        dec     a
        jr      nz,ECTop

        jr      CursorCleanup
