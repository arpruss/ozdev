global _ozsound,_ozquiet,_ozinitsound

psect text

; void ozinitsound(void)
_ozinitsound:
       ld      a,1
       out     (19h),a  ; turn tone mode on
       ret

; void ozsound(unsigned divisor);
_ozsound:
       call    _ozinitsound
       xor	a					
       out        (16h),a  ; turn off note
       ld      hl,2
       add     hl,sp
       ld      a,(hl)
       out     (17h),a
       inc     hl
       ld      a,(hl)
       out     (18h),a
       ld      a,3
       out     (16h),a  ; set frequency and output tone
       ret

_ozquiet:
	xor	a
	out	(16h),a  ; turn off note
global __ozclick_setting,_ozclick
        ld      a,(__ozclick_setting)
        or      a
        ret     z
        ld      hl,1
        push    hl
        call    _ozclick
        pop     hl
	ret

;ozspkoff:
;	 ld	 a,#0
;	 out	 (#0x19),a
;	 ret

