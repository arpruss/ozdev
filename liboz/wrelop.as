;	word relational	operation - returns flags correctly for
;       comparison of words in hl and de

;; modified by ARP

	psect	text
        global  wrelop,wrelop_ex

wrelop_ex:
        ex      de,hl
wrelop:
	ld	a,h
	xor	d		;compare signs
	jp	m,1f		;if different, return sign of lhs
	sbc	hl,de		;just set flags as normal
	ret
1:
	ld	a,h		;get sign of lhs
	and	80h		;mask out sign flag
	sbc	hl,de		;set carry flag if appropriate
	inc	a		;set sign flag as appropriate and reset Z flag
	ret

