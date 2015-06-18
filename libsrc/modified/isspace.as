	global	_isspace

	psect	text
_isspace:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
        xor     a
        cp      h               ;check for a char
	jr	nz,nix
        ld      a,l
        ld      l,h             ; h was zero--checked above
        inc     l               ; hl=1 now
	cp	' '
        ret     z               ;yes
        cp      12              ;newline [???]
        ret     z               ;yes
        cp      13
        ret     z
        cp      14
        ret     z
        cp      10
        ret     z
        cp      11
        ret     z
	cp	9		;tab
        ret     z               ;yes
        dec     l
        ret
nix:    ld      h,a
        ld      l,a
        ret

