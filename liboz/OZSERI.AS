;; buffered serial input

psect text
BufLen EQU 256

global __ozrxhandshaking,__ozrxxoff
global _ozserialgetc,__ozserbufget,__ozserbufput,_ozserinton,_ozserintoff
global $serial_hook
global $serial_check_hook
global $rxxoff_hook
global _ozsetrxhandshaking
global _ozgetrxhandshaking
global _ozclearserialbuffer

rxxoff_handler:
        ld      a,(__ozrxxoff)
        or      a
        jp      z,$rxxoff_hook+2
        ld      hl,__ozserbufput
        ld      a,(hl)
        dec     hl       ; hl=__ozserbufget
        sub     (hl)
        cp      150
        jp      nc,$rxxoff_hook+2
waittop2:
        in      a,(45h)
        and     20h
        jr      z,waittop2
        ld      a,17  ; XON
        out     (40h),a
        xor     a
        ld      (__ozrxxoff),a
        jp      $rxxoff_hook+2

serial_int_check:
        ld a,(__ozserbufget)
        ld c,a
        ld a,(__ozserbufput)
        cp c
        jp z,$serial_check_hook+2
        ei
        ret


serial_int:
        in      a,(45h)
        and     1
        jp      z,$serial_hook+2  ;; no serial data

        in      a,(40h)
        push    hl
        ld      e,a
        ld      hl,__ozserbufput
        ld      a,(hl)
        ld      c,a
        inc     a
        dec     hl  ; hl=__ozserbufget
        cp      (hl)
        jp      z,BufferFull
        inc     hl  ; hl=__ozserbufput
        ld      (hl),a
        ei

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
        jp      $serial_hook+2

_ozserinton:
        ld      hl,serial_int
        ld      ($serial_hook),hl
        ld      hl,serial_int_check
        ld      ($serial_check_hook),hl
        ld      hl,rxxoff_handler
        ld      ($rxxoff_hook),hl
        in a,(7)
        and 0ffh-4
        out (7),a
        ld a,1
        out (41h),a
        ret

_ozserintoff:
        ld      hl,$serial_hook+2
        ld      ($serial_hook),hl
        ld      hl,$serial_check_hook+2
        ld      ($serial_check_hook),hl
        ld      hl,$rxxoff_hook+2
        ld      ($rxxoff_hook),hl
        in a,(7)
        or 4
        out (7),a
        ld a,0
        out (41h),a
        ret

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

_ozclearserialbuffer:
        ld      hl,__ozserbufget
        ld      a,(hl)
        inc     hl
        ld      (hl),a
        xor     a
        ld      (__ozrxxoff),a
        ret

_ozsetrxhandshaking:
        pop     hl
        pop     de
        push    de

        ld      a,e
        or      a
        jr      z,load_it
        ld      a,0
load_it:
        ld      (__ozrxhandshaking),a

        xor     a
        ld      (__ozrxxoff),a

        jp      (hl)

_ozgetrxhandshaking:
        ld      a,(__ozrxhandshaking)
        ret

psect bss
__ozserbufget:
        defs 1
__ozserbufput:
        defs 1
SerialBuffer:
        defs BufLen
__ozrxhandshaking:
        defs 1
__ozrxxoff:
        defs 1

