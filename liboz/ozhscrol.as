        psect lowpage
        global _ozscrollright,_ozscrollleft,__ozactivepage,restore_a000

; void ozscrollright(byte y , byte rows)
; void ozscrollleft(byte y , byte rows)




_ozscrollright:

        call    _getparams
        call    _screen
_rowsr:
        xor      a                 ;used to set the carry flag to 0
        ld       b,30             ;set counter for columns
_columnsr:
        inc     hl               ;increment the screen byte counter
        rl      (hl)             ;shift screen byte to left through carry
        djnz    _columnsr         ;decrement column counter and branch if not zero
        res     7,(hl)           ;set column 239 to 0 to stop bleeding on RHS
        dec     c                ;decrement the row counter       
        jr      nz,_rowsr        ;branch if row not zero

        jp      restore_a000     ;restore memory page and exit



_ozscrollleft:                   ;like ozscrollright but in reverse
        call    _getparams 
        add     hl,bc
        call    _screen        

_rowsl:
        xor      a                 ;used to set the carry flag to 0
        ld       b,30             ;set counter for columns
_columnsl:
        rr      (hl)             ;shift screen byte to right through carry
        dec     hl               ;decrement the screen byte counter
        djnz    _columnsl         ;increment column counter and branch if not zero
        dec     c                ;decrement the row counter       
        jr      nz,_rowsl         ;branch if row not zero

        jp      restore_a000     ;restore memory page and exit


_getparams:

        pop     af
        pop     de
        pop     hl               ;get y
        pop     bc               ;get rows
        push    bc
        push    hl
        push    de
        push    af
        xor     a
        ld      h,a
        ld      b,a
        add     a,l
        cp      80
        jr      nc,_abort
        add     a,c
        cp       81
        ret     c
_abort:
        pop     hl
        ret        


_screen:        
                                 ;calc start row offset multiply y by 30
        add      hl,hl           ;*2
        ld       d,h             ;save
        ld       e,l 
        add      hl,hl           ;*4
        add      hl,hl           ;*8
        add      hl,hl           ;*16        
        add      hl,hl           ;*32
        sbc      hl,de           ;*30
        ld       de,0a000h-1
        adc      hl,de          ;add in the screen start position-1

        ld       de,(__ozactivepage)   ;page in the display memory
        ld       a,e
        out      (3),a
        ld       a,d
        out      (4),a
        ret
