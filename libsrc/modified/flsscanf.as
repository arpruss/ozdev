;a:ctype.h: 9: extern	unsigned char	_ctype_[];
;P:FLSSCANF.C: 6: typedef unsigned char uchar;
;P:FLSSCANF.C: 8: static char *   fp;
;P:FLSSCANF.C: 9: extern double	atof(char *);
;P:FLSSCANF.C: 10: extern int	atoi(char *);
;P:FLSSCANF.C: 18: static
;P:FLSSCANF.C: 19: range(c, base)
;P:FLSSCANF.C: 20: int	c;
;P:FLSSCANF.C: 21: uchar	base;
;P:FLSSCANF.C: 22: {
psect	text
_range:
global	ncsv, cret, indir
call	ncsv
defw	f4
;P:FLSSCANF.C: 23: 	if(	((_ctype_+1)[c]&0x04))
global	__ctype_
ld	e,(ix+6)
ld	d,(ix+1+6)
ld	hl,__ctype_+1
add	hl,de
bit	2&7,(hl)
jp	z,l2
;P:FLSSCANF.C: 24: 		c -= '0';
ld	de,-48
ld	l,(ix+6)
ld	h,(ix+1+6)
add	hl,de
ld	(ix+6),l
ld	(ix+1+6),h
;P:FLSSCANF.C: 25: 	else
jp	l3
l2:
;P:FLSSCANF.C: 26: 		{
;P:FLSSCANF.C: 27: 		if (	((_ctype_+1)[c]&0x01))
ld	e,(ix+6)
ld	d,(ix+1+6)
ld	hl,__ctype_+1
add	hl,de
bit	0&7,(hl)
jp	z,l4
;P:FLSSCANF.C: 28:             c += 'a'-'A' ;
ld	de,32
ld	l,(ix+6)
ld	h,(ix+1+6)
add	hl,de
ld	(ix+6),l
ld	(ix+1+6),h
;P:FLSSCANF.C: 29: 		if (	((_ctype_+1)[c]&(0x01|0x02)))
l4:
ld	e,(ix+6)
ld	d,(ix+1+6)
ld	hl,__ctype_+1
add	hl,de
ld	a,(hl)
and	.low.3
or	a
jp	lz,l5
;P:FLSSCANF.C: 30: 			c = c - 'a' + 10 ;
ld	e,(ix+6)
ld	d,(ix+1+6)
ld	hl,-87
add	hl,de
ld	(ix+6),l
ld	(ix+1+6),h
;P:FLSSCANF.C: 31: 		else
jp	l6
l5:
;P:FLSSCANF.C: 32: 			return -1 ;
ld	hl,-1
jp	l1
l6:
;P:FLSSCANF.C: 33: 		}
l3:
;P:FLSSCANF.C: 34: 	if (c >= base)
ld	e,(ix+8)
ld	d,0
ld	l,(ix+6)
ld	h,(ix+1+6)
global	wrelop
call	wrelop
jp	llt,l7
;P:FLSSCANF.C: 35: 		return -1 ;
ld	hl,-1
jp	l1
;P:FLSSCANF.C: 36: 	return c ;
l7:
ld	l,(ix+6)
ld	h,(ix+1+6)
jp	l1
;P:FLSSCANF.C: 37: }
l1:
jp	cret
f4	equ	0
;P:FLSSCANF.C: 39: static
;P:FLSSCANF.C: 40: wspace()
;P:FLSSCANF.C: 41: {
_wspace:
call	ncsv
defw	f6
;P:FLSSCANF.C: 42: 	int	c;
;P:FLSSCANF.C: 44: 	while(	((_ctype_+1)[c =   (*(fp)++)]&0x08))
jp	l9
l10:
;P:FLSSCANF.C: 45: 		continue;
jp	l9
l9:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	e,a
rla
sbc	a,a
ld	d,a
ld	(ix+-2),e
ld	(ix+1+-2),d
ld	hl,__ctype_+1
add	hl,de
bit	3&7,(hl)
jp	nz,l10
l11:
;P:FLSSCANF.C: 46: 	if(c != (-1))
ld	de,-1
ld	l,(ix+-2)
ld	h,(ix+1+-2)
or	a
sbc	hl,de
jp	z,l12
;P:FLSSCANF.C: 47: 		 (*(--( fp))=c);
ld	a,(ix+-2)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 48: }
l12:
l8:
jp	cret
f6	equ	-2
;P:FLSSCANF.C: 50: sscanf(string, fmt, a)
;P:FLSSCANF.C: 51: char *      string;
;P:FLSSCANF.C: 52: register char *	fmt;
;P:FLSSCANF.C: 53: int *a;
;P:FLSSCANF.C: 54: {
global	_sscanf
_sscanf:
call	ncsv
defw	f7
ld	l,(ix+8)
ld	h,(ix+9)
push	hl
pop	iy
;P:FLSSCANF.C: 55:     static uchar   c, sign, base, n, noass,len;
;P:FLSSCANF.C: 56:     static char    width ;
;P:FLSSCANF.C: 57:     static char *  sptr;
;P:FLSSCANF.C: 58:     static int ch;
;P:FLSSCANF.C: 59:     static long    val;
;P:FLSSCANF.C: 60:     static char    buf[60];
;P:FLSSCANF.C: 61:     static int **      args;
;P:FLSSCANF.C: 62:     args=&a;
push	ix
pop	de
ld	hl,10
add	hl,de
ld	(F28),hl
;P:FLSSCANF.C: 64:     fp = string;
ld	l,(ix+6)
ld	h,(ix+1+6)
ld	(_fp),hl
;P:FLSSCANF.C: 65: 	n = 0;
ld	a,.low.0
ld	(F20),a
;P:FLSSCANF.C: 66: 	while(c = *fmt++) {
jp	l14
l15:
;P:FLSSCANF.C: 68: 		len = 0 ;
ld	a,.low.0
ld	(F22),a
;P:FLSSCANF.C: 69: 		if(	((_ctype_+1)[c]&0x08)) {
ld	a,(F17)
ld	e,a
ld	d,0
ld	hl,__ctype_+1
add	hl,de
bit	3&7,(hl)
jp	z,l17
;P:FLSSCANF.C: 70: 			wspace();
call	_wspace
ld	hl,0
add	hl,sp
ld	sp,hl
;P:FLSSCANF.C: 71: 			continue;
jp	l14
;P:FLSSCANF.C: 72: 		}
;P:FLSSCANF.C: 73: 		if(c == '%') {
l17:
ld	a,(F17)
cp	.low.37
jp	nz,l18
;P:FLSSCANF.C: 74: 			noass = 0;
ld	a,.low.0
ld	(F21),a
;P:FLSSCANF.C: 75: 			width = 0;
ld	a,.low.0
ld	(F23),a
;P:FLSSCANF.C: 76: loop:
l19:
;P:FLSSCANF.C: 77: 			switch(c = *fmt++) {
jp	l21
;P:FLSSCANF.C: 79: 			case '\0':
l22:
;P:FLSSCANF.C: 80: 				return n ? n :   (!*(fp)) ? (-1) : 0;
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,(_fp)
ld	a,(hl)
or	a
jp	az,40f
jp	41f
41:
ld	hl,0
jp	42f
40:
ld	hl,-1
42:
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 82: 			case '*':
l23:
;P:FLSSCANF.C: 83: 				noass++;
ld	a,(F21)
add	a,.low.1
ld	(F21),a
;P:FLSSCANF.C: 84: 				goto loop;
jp	l19
;P:FLSSCANF.C: 86: 			case 'F':
l24:
;P:FLSSCANF.C: 87: 				len++;
ld	a,(F22)
add	a,.low.1
ld	(F22),a
;P:FLSSCANF.C: 88: 			case 'f':
l25:
;P:FLSSCANF.C: 89: 				wspace();
call	_wspace
ld	hl,0
add	hl,sp
ld	sp,hl
;P:FLSSCANF.C: 90: 				sptr = buf;
ld	hl,F27
ld	(F24),hl
;P:FLSSCANF.C: 91: 				if(width == 0)
ld	a,(F23)
or	a
jp	anz,l26
;P:FLSSCANF.C: 92: 					width = sizeof buf - 1;
ld	a,.low.59
ld	(F23),a
;P:FLSSCANF.C: 93: 				sign = 0;
l26:
ld	a,.low.0
ld	(F18),a
;P:FLSSCANF.C: 94: 				ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 95: 				if(ch == '-') {
ld	de,45
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,l27
;P:FLSSCANF.C: 96: 					*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 97: 					ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 98: 					width--;
ld	a,(F23)
sub	.low.1
ld	(F23),a
;P:FLSSCANF.C: 99: 				}
;P:FLSSCANF.C: 100: 				while(width && 	((_ctype_+1)[ch]&0x04) || !sign && ch == '.') {
l27:
jp	l28
l29:
;P:FLSSCANF.C: 101: 					*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 102: 					if(ch == '.')
ld	de,46
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,l31
;P:FLSSCANF.C: 103: 						sign++;
ld	a,(F18)
add	a,.low.1
ld	(F18),a
;P:FLSSCANF.C: 104: 					ch =   (*(fp)++);
l31:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 105: 					width--;
ld	a,(F23)
sub	.low.1
ld	(F23),a
;P:FLSSCANF.C: 106: 				}
l28:
ld	a,(F23)
or	a
jp	anz,30f
jp	31f
30:
ld	de,(F25)
ld	hl,__ctype_+1
add	hl,de
bit	2&7,(hl)
jp	nz,30f
jp	31f
31:jp	21f
30:jp	20f
21:
ld	a,(F18)
or	a
jp	lz,30f
jp	31f
30:
ld	de,46
ld	hl,(F25)
or	a
sbc	hl,de
jp	z,30f
jp	31f
31:jp	21f
30:jp	20f
21:jp	11f
20:jp	10f
10:
jp	l29
11:
l30:
;P:FLSSCANF.C: 107: 				if(width && (ch == 'e' || ch == 'E')) {
ld	a,(F23)
or	a
jp	az,20f
jp	21f
21:
ld	de,101
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,30f
jp	31f
30:
ld	de,69
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,30f
jp	31f
31:jp	21f
30:jp	20f
21:jp	11f
20:jp	10f
10:
jp	l32
11:
;P:FLSSCANF.C: 108: 					*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 109: 					ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 110: 					width--;
ld	a,(F23)
sub	.low.1
ld	(F23),a
;P:FLSSCANF.C: 111: 					if(width && (ch == '-' || ch == '+')) {
ld	a,(F23)
or	a
jp	az,20f
jp	21f
21:
ld	de,45
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,30f
jp	31f
30:
ld	de,43
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,30f
jp	31f
31:jp	21f
30:jp	20f
21:jp	11f
20:jp	10f
10:
jp	l33
11:
;P:FLSSCANF.C: 112: 						*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 113: 						ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 114: 						width--;
ld	a,(F23)
sub	.low.1
ld	(F23),a
;P:FLSSCANF.C: 115: 					}
;P:FLSSCANF.C: 116: 					while(width && 	((_ctype_+1)[ch]&0x04)) {
l33:
jp	l34
l35:
;P:FLSSCANF.C: 117: 						*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 118: 						ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 119: 						width--;
ld	a,(F23)
sub	.low.1
ld	(F23),a
;P:FLSSCANF.C: 120: 					}
l34:
ld	a,(F23)
or	a
jp	anz,20f
jp	21f
20:
ld	de,(F25)
ld	hl,__ctype_+1
add	hl,de
bit	2&7,(hl)
jp	nz,20f
jp	21f
21:jp	11f
20:jp	10f
10:
jp	l35
11:
l36:
;P:FLSSCANF.C: 121: 				}
;P:FLSSCANF.C: 122: 				*sptr = 0;
l32:
ld	hl,(F24)
ld	(hl),0
;P:FLSSCANF.C: 123:                 if(ch)
ld	hl,(F25)
ld	a,l
or	h
jp	z,l37
;P:FLSSCANF.C: 124: 					 (*(--( fp))=ch);
ld	a,(F25)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 125: 				if(sptr == buf)
l37:
ld	de,F27
ld	hl,(F24)
or	a
sbc	hl,de
jp	nz,l38
;P:FLSSCANF.C: 126: 					return n ? n :   (!*(fp)) ? (-1) : 0;
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,(_fp)
ld	a,(hl)
or	a
jp	az,40f
jp	41f
41:
ld	hl,0
jp	42f
40:
ld	hl,-1
42:
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 127: 				n++;
l38:
ld	a,(F20)
add	a,.low.1
ld	(F20),a
;P:FLSSCANF.C: 128: 				if(!noass)
ld	a,(F21)
or	a
jp	lnz,l39
;P:FLSSCANF.C: 129: 					if(len)
ld	a,(F22)
or	a
jp	lz,l40
;P:FLSSCANF.C: 130: 						*(double *)*args++ = atof(buf);
global	_atof
ld	hl,F27
push	hl
call	_atof
exx
ld	hl,2
add	hl,sp
ld	sp,hl
exx
push	hl
push	de
ld	hl,(F28)
inc	hl
inc hl
ld	(F28),hl
dec	hl
dec hl
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
pop	de
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
pop	bc
ld	(hl),c
inc	hl
ld	(hl),b
ld	l,c
ld	h,b
;P:FLSSCANF.C: 131: 					else
jp	l41
l40:
;P:FLSSCANF.C: 132: 						*(float *)*args++ = atof(buf);
ld	hl,F27
push	hl
call	_atof
exx
ld	hl,2
add	hl,sp
ld	sp,hl
exx
push	hl
push	de
ld	hl,(F28)
inc	hl
inc hl
ld	(F28),hl
dec	hl
dec hl
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
pop	de
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
pop	bc
ld	(hl),c
inc	hl
ld	(hl),b
ld	l,c
ld	h,b
l41:
;P:FLSSCANF.C: 133: 				continue;
l39:
jp	l14
;P:FLSSCANF.C: 135: 			case 'l':
l42:
;P:FLSSCANF.C: 136: 				len++;
ld	a,(F22)
add	a,.low.1
ld	(F22),a
;P:FLSSCANF.C: 137: 				goto loop;
jp	l19
;P:FLSSCANF.C: 139: 			case 'D':
l43:
;P:FLSSCANF.C: 140: 				len++;
ld	a,(F22)
add	a,.low.1
ld	(F22),a
;P:FLSSCANF.C: 141: 			case 'd':
l44:
;P:FLSSCANF.C: 142: 				base = 10;
ld	a,.low.10
ld	(F19),a
;P:FLSSCANF.C: 143: 				break;
jp	l20
;P:FLSSCANF.C: 145: 			case 'O':
l45:
;P:FLSSCANF.C: 146: 				len++;
ld	a,(F22)
add	a,.low.1
ld	(F22),a
;P:FLSSCANF.C: 147: 			case 'o':
l46:
;P:FLSSCANF.C: 148: 				base = 8;
ld	a,.low.8
ld	(F19),a
;P:FLSSCANF.C: 149: 				break;
jp	l20
;P:FLSSCANF.C: 151: 			case 'X':
l47:
;P:FLSSCANF.C: 152: 				len++;
ld	a,(F22)
add	a,.low.1
ld	(F22),a
;P:FLSSCANF.C: 153: 			case 'x':
l48:
;P:FLSSCANF.C: 154: 				base = 16;
ld	a,.low.16
ld	(F19),a
;P:FLSSCANF.C: 155: 				break ;
jp	l20
;P:FLSSCANF.C: 157: 			case 's':
l49:
;P:FLSSCANF.C: 158: 				wspace();
call	_wspace
ld	hl,0
add	hl,sp
ld	sp,hl
;P:FLSSCANF.C: 159: 				if ( !noass )
ld	a,(F21)
or	a
jp	lnz,l50
;P:FLSSCANF.C: 160: 					sptr = (char *)*args++;
ld	hl,(F28)
ld	c,(hl)
inc	hl
ld	b,(hl)
dec	hl
inc	hl
inc hl
ld	(F28),hl
ld	(F24),bc
;P:FLSSCANF.C: 161:                 if (!(ch =   (*(fp)++)))
l50:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
ld	a,l
or	h
jp	nz,l51
;P:FLSSCANF.C: 162: 					return n ? n : (-1);
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,-1
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 163:                 while(ch && !	((_ctype_+1)[ch]&0x08)) {
l51:
jp	l52
l53:
;P:FLSSCANF.C: 164: 					if(ch == *fmt) {
ld	de,(F25)
ld	a,(iy+0)
ld	l,a
rla
sbc	a,a
ld	h,a
or	a
sbc	hl,de
jp	nz,l55
;P:FLSSCANF.C: 165: 						fmt++;
inc	iy
;P:FLSSCANF.C: 166: 						break;
jp	l54
;P:FLSSCANF.C: 167: 					}
;P:FLSSCANF.C: 168: 					if ( !noass ) 
l55:
ld	a,(F21)
or	a
jp	lnz,l56
;P:FLSSCANF.C: 169: 						*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 170: 					if(--width == 0)
l56:
ld	a,(F23)
add	a,.low.-1
ld	(F23),a
or	a
jp	anz,l57
;P:FLSSCANF.C: 171: 						break;
jp	l54
;P:FLSSCANF.C: 172: 					ch =   (*(fp)++);
l57:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 173: 				}
l52:
ld	hl,(F25)
ld	a,l
or	h
jp	nz,20f
jp	21f
20:
ld	de,(F25)
ld	hl,__ctype_+1
add	hl,de
bit	3&7,(hl)
jp	z,20f
jp	21f
21:jp	11f
20:jp	10f
10:
jp	l53
11:
l54:
;P:FLSSCANF.C: 174: 				n++;
ld	a,(F20)
add	a,.low.1
ld	(F20),a
;P:FLSSCANF.C: 175: 				if(!noass)
ld	a,(F21)
or	a
jp	lnz,l58
;P:FLSSCANF.C: 176: 					*sptr = 0;
ld	hl,(F24)
ld	(hl),0
;P:FLSSCANF.C: 177: 				continue;
l58:
jp	l14
;P:FLSSCANF.C: 179: 			case 'c':
l59:
;P:FLSSCANF.C: 180: 				if ( !noass )
ld	a,(F21)
or	a
jp	lnz,l60
;P:FLSSCANF.C: 181: 					sptr = (char *)*args++;
ld	hl,(F28)
ld	c,(hl)
inc	hl
ld	b,(hl)
dec	hl
inc	hl
inc hl
ld	(F28),hl
ld	(F24),bc
;P:FLSSCANF.C: 182: 				do {
l60:
l63:
;P:FLSSCANF.C: 183:                     if (!(ch =   (*(fp)++)))
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
ld	a,l
or	h
jp	nz,l64
;P:FLSSCANF.C: 184: 						return n ? n : (-1);
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,-1
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 185: 					if ( !noass )
l64:
ld	a,(F21)
or	a
jp	lnz,l65
;P:FLSSCANF.C: 186: 						*sptr++ = ch;
ld	a,(F25)
ld	hl,(F24)
inc	hl
ld	(F24),hl
dec	hl
ld	(hl),a
;P:FLSSCANF.C: 187: 				} while(--width > 0);
l65:
l61:
ld	a,(F23)
add	a,.low.-1
ld	(F23),a
ld	e,a
rla
sbc	a,a
ld	d,a
ld	hl,0
global	wrelop
call	wrelop
jp	alt,l63
l62:
;P:FLSSCANF.C: 188: 				n++;
ld	a,(F20)
add	a,.low.1
ld	(F20),a
;P:FLSSCANF.C: 189: 				continue;
jp	l14
;P:FLSSCANF.C: 190: 			default:
l66:
;P:FLSSCANF.C: 191: 				if(	((_ctype_+1)[c]&0x04)) {
ld	a,(F17)
ld	e,a
ld	d,0
ld	hl,__ctype_+1
add	hl,de
bit	2&7,(hl)
jp	z,l67
;P:FLSSCANF.C: 192: 					width = atoi(fmt-1);
global	_atoi
push	iy
pop	hl
dec	hl
push	hl
call	_atoi
exx
ld	hl,2
add	hl,sp
ld	sp,hl
exx
ld	l,l
ld	h,h
ld	a,l
ld	(F23),a
;P:FLSSCANF.C: 193: 					while(	((_ctype_+1)[*fmt]&0x04))
jp	l68
l69:
;P:FLSSCANF.C: 194: 						fmt++;
inc	iy
l68:
ld	a,(iy+0)
ld	e,a
rla
sbc	a,a
ld	d,a
ld	hl,__ctype_+1
add	hl,de
bit	2&7,(hl)
jp	nz,l69
l70:
;P:FLSSCANF.C: 195: 					goto loop;
jp	l19
;P:FLSSCANF.C: 196: 				}
;P:FLSSCANF.C: 197: 				if(c != (ch =   (*(fp)++)))
l67:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	e,a
rla
sbc	a,a
ld	d,a
ld	(F25),de
ld	a,(F17)
ld	l,a
ld	h,0
or	a
sbc	hl,de
jp	z,l71
;P:FLSSCANF.C: 198:                     if(!ch)
ld	hl,(F25)
ld	a,l
or	h
jp	nz,l72
;P:FLSSCANF.C: 199: 						return n ? n : (-1);
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,-1
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 200: 					else {
jp	l73
l72:
;P:FLSSCANF.C: 201: 						 (*(--( fp))=ch);
ld	a,(F25)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 202: 						return n;
ld	a,(F20)
ld	l,a
ld	h,0
jp	l13
;P:FLSSCANF.C: 203: 					}
l73:
;P:FLSSCANF.C: 204: 				continue;
l71:
jp	l14
;P:FLSSCANF.C: 205: 			}
jp	l20
l21:
ld	a,(iy+0)
inc	iy
ld	(F17),a
or	a
jp	z,l22
cp	42
jp	z,l23
cp	68
jp	z,l43
cp	70
jp	z,l24
cp	79
jp	z,l45
cp	88
jp	z,l47
cp	99
jp	z,l59
cp	100
jp	z,l44
cp	102
jp	z,l25
cp	108
jp	z,l42
cp	111
jp	z,l46
cp	115
jp	z,l49
cp	120
jp	z,l48
jp	l66
l20:
;P:FLSSCANF.C: 206: 			wspace();
call	_wspace
ld	hl,0
add	hl,sp
ld	sp,hl
;P:FLSSCANF.C: 207: 			val = 0;
ld	de,0
ld	hl,0
ld	(F26),de
ld	(F26+2),hl
;P:FLSSCANF.C: 208: 			sign = 0;
ld	a,.low.0
ld	(F18),a
;P:FLSSCANF.C: 209: 			ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 210: 			if(ch == '-') {
ld	de,45
ld	hl,(F25)
or	a
sbc	hl,de
jp	nz,l74
;P:FLSSCANF.C: 211: 				sign++;
ld	a,(F18)
add	a,.low.1
ld	(F18),a
;P:FLSSCANF.C: 212: 				ch =   (*(fp)++);
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
;P:FLSSCANF.C: 213: 			}
;P:FLSSCANF.C: 214: 			if(range(ch, base) == -1) {
l74:
ld	a,(F19)
ld	l,a
ld	h,0
push	hl
ld	hl,(F25)
push	hl
call	_range
exx
ld	hl,2+2
add	hl,sp
ld	sp,hl
exx
ld	l,l
ld	h,h
ld	de,-1
or	a
sbc	hl,de
jp	nz,l75
;P:FLSSCANF.C: 215: 				 (*(--( fp))=ch);
ld	a,(F25)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 216: 				return n ? n :   (!*(fp)) ? (-1) : 0;
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,(_fp)
ld	a,(hl)
or	a
jp	az,40f
jp	41f
41:
ld	hl,0
jp	42f
40:
ld	hl,-1
42:
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
;P:FLSSCANF.C: 217: 			}
;P:FLSSCANF.C: 218: 			do {
l75:
l78:
;P:FLSSCANF.C: 219: 				val = val * base + range(ch, base);
ld	a,(F19)
ld	l,a
ld	h,0
push	hl
ld	hl,(F25)
push	hl
call	_range
exx
ld	hl,2+2
add	hl,sp
ld	sp,hl
exx
ld	e,l
ld	d,h
ld	hl,0
push	hl
push	de
ld	de,(F26)
ld	hl,(F26+2)
push	hl
push	de
ld	a,(F19)
ld	hl,0
ld	d,l
ld	e,a
global	llmul
call	llmul
global	lladd
call	lladd
ld	(F26),de
ld	(F26+2),hl
;P:FLSSCANF.C: 220: 			} while (( --width != 0 ) && ( range(ch =   (*(fp)++),base) != -1 )) ;
l76:
ld	a,(F23)
add	a,.low.-1
ld	(F23),a
or	a
jp	anz,20f
jp	21f
20:
ld	a,(F19)
ld	l,a
ld	h,0
push	hl
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	l,a
rla
sbc	a,a
ld	h,a
ld	(F25),hl
push	hl
call	_range
exx
ld	hl,2+2
add	hl,sp
ld	sp,hl
exx
ld	l,l
ld	h,h
ld	de,-1
or	a
sbc	hl,de
jp	nz,20f
jp	21f
21:jp	11f
20:jp	10f
10:
jp	l78
11:
l77:
;P:FLSSCANF.C: 221: 			n++;
ld	a,(F20)
add	a,.low.1
ld	(F20),a
;P:FLSSCANF.C: 222: 			if (range(ch,base) == -1)
ld	a,(F19)
ld	l,a
ld	h,0
push	hl
ld	hl,(F25)
push	hl
call	_range
exx
ld	hl,2+2
add	hl,sp
ld	sp,hl
exx
ld	l,l
ld	h,h
ld	de,-1
or	a
sbc	hl,de
jp	nz,l79
;P:FLSSCANF.C: 223: 				 (*(--( fp))=ch);
ld	a,(F25)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 224: 			if(sign)
l79:
ld	a,(F18)
or	a
jp	lz,l80
;P:FLSSCANF.C: 225: 				val = -val;
ld	de,(F26)
ld	hl,(F26+2)
push	hl
push	de
ld	hl,0
pop	bc
or	a
sbc	hl,bc
pop	bc
ex	de,hl
ld	hl,0
sbc	hl,bc
ld	(F26),de
ld	(F26+2),hl
;P:FLSSCANF.C: 226: 			if ( !noass )
l80:
ld	a,(F21)
or	a
jp	lnz,l81
;P:FLSSCANF.C: 227: 				if(len)
ld	a,(F22)
or	a
jp	lz,l82
;P:FLSSCANF.C: 228: 					*(long *)*args++ = val;
ld	de,(F26)
ld	hl,(F26+2)
push	hl
push	de
ld	hl,(F28)
inc	hl
inc hl
ld	(F28),hl
dec	hl
dec hl
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
pop	de
ld	(hl),e
inc	hl
ld	(hl),d
inc	hl
pop	bc
ld	(hl),c
inc	hl
ld	(hl),b
ld	l,c
ld	h,b
;P:FLSSCANF.C: 229: 				else
jp	l83
l82:
;P:FLSSCANF.C: 230: 					**args++ = val;
ld	de,(F26)
ld	hl,(F28)
inc	hl
inc hl
ld	(F28),hl
dec	hl
dec hl
ld	a,(hl)
inc	hl
ld	h,(hl)
ld	l,a
ld	(hl),e
inc	hl
ld	(hl),d
l83:
;P:FLSSCANF.C: 231: 			continue;
l81:
jp	l14
;P:FLSSCANF.C: 232: 		} else if(c != (ch =   (*(fp)++))) {
jp	l84
l18:
ld	hl,(_fp)
ld	a,(hl)
inc	hl
ld	(_fp),hl
ld	e,a
rla
sbc	a,a
ld	d,a
ld	(F25),de
ld	a,(F17)
ld	l,a
ld	h,0
or	a
sbc	hl,de
jp	z,l85
;P:FLSSCANF.C: 233:             if(ch) {
ld	hl,(F25)
ld	a,l
or	h
jp	z,l86
;P:FLSSCANF.C: 234: 				 (*(--( fp))=ch);
ld	a,(F25)
ld	hl,(_fp)
dec	hl
ld	(_fp),hl
ld	(hl),a
;P:FLSSCANF.C: 235: 				return n;
ld	a,(F20)
ld	l,a
ld	h,0
jp	l13
;P:FLSSCANF.C: 236: 			} else
jp	l87
l86:
;P:FLSSCANF.C: 237: 				return n ? n : (-1);
ld	a,(F20)
or	a
jp	lnz,20f
jp	21f
21:
ld	hl,-1
jp	22f
20:
ld	a,(F20)
ld	l,a
ld	h,0
22:
jp	l13
l87:
;P:FLSSCANF.C: 238: 		}
;P:FLSSCANF.C: 239: 	}
l85:
l84:
l14:
ld	a,(iy+0)
inc	iy
ld	(F17),a
or	a
jp	lnz,l15
l16:
;P:FLSSCANF.C: 240: 	return n;
ld	a,(F20)
ld	l,a
ld	h,0
jp	l13
;P:FLSSCANF.C: 241: }
l13:
psect	bss
F20:
	defs	1
F21:
	defs	1
F22:
	defs	1
F23:
	defs	1
F24:
	defs	2
F25:
	defs	2
F17:
	defs	1
F26:
	defs	4
F18:
	defs	1
F27:
	defs	60
F19:
	defs	1
F28:
	defs	2
psect	text
jp	cret
f7	equ	0
psect	bss
_fp:
	defs	2
F20)
ld	l,a
ld	h,0
22:
jp	l13
l87:
;P:FLSSCANF.C: 23