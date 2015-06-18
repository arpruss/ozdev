global	_ozpoint
global	ncsv, cret, indir
global	lmul
global	shll
global	wrelop
global	_ozdisplayorbyte
psect	text
_ozpoint:
global csv
call csv
push hl
push hl
ld	de,30
ld	l,(ix+8)
ld	h,(ix+9)
call	lmul
ld	e,(ix+6)
ld	d,(ix+7)
srl	d
rr	e
srl	d
rr	e
srl	d
rr	e
add	hl,de
ld	(ix+-2),l
ld	(ix+-1),h
ld	a,(ix+6)
and	7
ld	b,a
ld	hl,1
call	shll
ld	(ix+-4),l
ld	(ix+-3),h
bit	7,(ix+-1)
jp	nz,cret
ld	de,2400
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	p,cret
ld	l,(ix+-4)
ld	h,(ix+-3)
push	hl
ld	l,(ix+-2)
ld	h,(ix+-1)
push	hl
call	_ozdisplayorbyte
jp	cret
global	_exit
_exit:
ret	
global	_close
_close:
ret	
global	_main
global	_sin
global	_cos
psect	data
e59:
deff	39
e61:
deff	20
e60:
deff	2
e57:
deff	3.1415926536
e56:
deff	240.
global	aitof
global	fldiv
global	flmul
global	flmul
global	flmul
e62:
deff	5
e58:
deff	240.
global	aitof
global	fldiv
global	flmul
global	flsub
global	fladd
global	ftol
global	wrelop
global	_sprintf
e63:
deff	4.5
global	_ozputs
e67:
deff	120
e68:
deff	30
e66:
deff	3.1415926536
e69:
deff	2
e65:
deff	360.
global	aitof
global	fldiv
global	flmul
global	flmul
global	flmul
global	fladd
e72:
deff	40
e73:
deff	30
e71:
deff	3.1415926536
e74:
deff	2
e70:
deff	360.
global	aitof
global	fldiv
global	flmul
global	flmul
global	flmul
global	fladd
global	ftol
global	ftol
global	wrelop
global	_sqrt
e75:
deff	240.
global	aitof
global	fldiv
e76:
deff	80
global	flmul
global	ftol
global	wrelop
e77:
deff	240.
global	aitof
global	fldiv
e78:
deff	80
global	flmul
global	ftol
global	wrelop
psect	text
_main:
call ncsv
defw -102
ld	(ix+-2),0
ld	(ix+-1),0
jp	l10
l7:
ld	de,(e59)
ld	hl,(e59+2)
push	hl
push	de
ld	de,(e61)
ld	hl,(e61+2)
push	hl
push	de
ld	de,(e60)
ld	hl,(e60+2)
push	hl
push	de
ld	de,(e57)
ld	hl,(e57+2)
push	hl
push	de
ld	de,(e56)
ld	hl,(e56+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
call	flmul
call	flmul
push	hl
push	de
call	_sin
pop	bc
pop	bc
call	flmul
push	hl
push	de
ld	de,(e62)
ld	hl,(e62+2)
push	hl
push	de
ld	de,(e58)
ld	hl,(e58+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
push	hl
push	de
call	_cos
pop	bc
pop	bc
call	flmul
call	flsub
call	fladd
ld	(ix+-86),e
ld	(ix+-85),d
ld	(ix+-84),l
ld	(ix+-83),h
call	ftol
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
push	hl
call	_ozpoint
pop	bc
pop	bc
ld	l,(ix+-2)
ld	h,(ix+-1)
inc	hl
ld	(ix+-2),l
ld	(ix+-1),h
l10:
ld	de,240
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	m,l7
ld	de,(e63)
ld	hl,(e63+2)
push	hl
push	de
call	_cos
pop	bc
ex	(sp),hl
push	de
ld	hl,19f
push	hl
push	ix
pop	de
ld	hl,-82
add	hl,de
push	hl
call	_sprintf
pop	bc
pop	bc
pop	bc
push	ix
pop	de
ld	hl,-82
add	hl,de
ex	(sp),hl
ld	hl,10
push	hl
push	hl
call	_ozputs
pop	bc
pop	bc
pop	bc
ld	(ix+-2),0
ld	(ix+-1),0
jp	l14
l11:
ld	de,(e67)
ld	hl,(e67+2)
push	hl
push	de
ld	de,(e68)
ld	hl,(e68+2)
push	hl
push	de
ld	de,(e66)
ld	hl,(e66+2)
push	hl
push	de
ld	de,(e69)
ld	hl,(e69+2)
push	hl
push	de
ld	de,(e65)
ld	hl,(e65+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
call	flmul
call	flmul
push	hl
push	de
call	_sin
pop	bc
pop	bc
call	flmul
call	fladd
ld	(ix+-90),e
ld	(ix+-89),d
ld	(ix+-88),l
ld	(ix+-87),h
ld	de,(e72)
ld	hl,(e72+2)
push	hl
push	de
ld	de,(e73)
ld	hl,(e73+2)
push	hl
push	de
ld	de,(e71)
ld	hl,(e71+2)
push	hl
push	de
ld	de,(e74)
ld	hl,(e74+2)
push	hl
push	de
ld	de,(e70)
ld	hl,(e70+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
call	flmul
call	flmul
push	hl
push	de
call	_cos
pop	bc
pop	bc
call	flmul
call	fladd
ld	(ix+-94),e
ld	(ix+-93),d
ld	(ix+-92),l
ld	(ix+-91),h
call	ftol
push	de
ld	e,(ix+-90)
ld	d,(ix+-89)
ld	l,(ix+-88)
ld	h,(ix+-87)
call	ftol
push	de
call	_ozpoint
pop	bc
pop	bc
ld	l,(ix+-2)
ld	h,(ix+-1)
inc	hl
ld	(ix+-2),l
ld	(ix+-1),h
l14:
ld	de,360
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	m,l11
ld	(ix+-2),0
ld	(ix+-1),0
jp	l18
l15:
ld	de,(e75)
ld	hl,(e75+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
push	hl
push	de
call	_sqrt
pop	bc
pop	bc
ld	(ix+-98),e
ld	(ix+-97),d
ld	(ix+-96),l
ld	(ix+-95),h
ld	de,(e76)
ld	hl,(e76+2)
push	hl
push	de
ld	e,(ix+-98)
ld	d,(ix+-97)
ld	l,(ix+-96)
ld	h,(ix+-95)
call	flmul
call	ftol
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
push	hl
call	_ozpoint
pop	bc
pop	bc
ld	l,(ix+-2)
ld	h,(ix+-1)
inc	hl
ld	(ix+-2),l
ld	(ix+-1),h
l18:
ld	de,240
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	m,l15
ld	(ix+-2),0
ld	(ix+-1),0
jp	l22
l19:
ld	de,(e77)
ld	hl,(e77+2)
push	hl
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
call	aitof
call	fldiv
push	hl
push	de
call	_sqrt
pop	bc
pop	bc
ld	(ix+-102),e
ld	(ix+-101),d
ld	(ix+-100),l
ld	(ix+-99),h
ld	de,(e78)
ld	hl,(e78+2)
push	hl
push	de
ld	e,(ix+-102)
ld	d,(ix+-101)
ld	l,(ix+-100)
ld	h,(ix+-99)
call	flmul
call	ftol
push	de
ld	l,(ix+-2)
ld	h,(ix+-1)
push	hl
call	_ozpoint
pop	bc
pop	bc
ld	l,(ix+-2)
ld	h,(ix+-1)
inc	hl
ld	(ix+-2),l
ld	(ix+-1),h
l22:
ld	de,240
ld	l,(ix+-2)
ld	h,(ix+-1)
call	wrelop
jp	m,l19
jp	cret
psect	data
19:
defb	86,97,108,117,101,32,61,32,37,103,0
psect	text
all	_sqrt
pop	bc
pop	bc
l