FLASH_RATE equ 16

psect lowpage
global _cursor_active,_cursor_count,_cursor_pos,_cursor_byte1,_cursor_byte2
global _doxorcursor,_cursor_state,_cursor_height
global _installcursor
global $64hz_hook,_oz64hztimeron
_cursor_state: defb 0
_cursor_active: defb 0
_cursor_count: defb FLASH_RATE
_cursor_pos: defw 0a000h
_cursor_height: defb 0
cursor_storage: defs 2*15

_installcursor:
        di
        ld      a,call
        ld      ($64hz_hook-1),a
        ld      hl,FlashCursor
        ld      ($64hz_hook),hl
        ei
        jp      _oz64hztimeron

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
