;othello.h: 10: typedef unsigned char boardchar;
;othello.h: 12: typedef struct boardtype
;othello.h: 13: {
;othello.h: 14:     struct boardtype *sons[30];
;othello.h: 15:     int val;
;othello.h: 16:     boardchar *array[8];
;othello.h: 17: }
;othello.h: 18: BOARD;
;othello.h: 20: int worth_1(BOARD *board);
;othello.h: 21: int worth_2(BOARD *board);
;othello.h: 22: int allsons(BOARD *pos);
;othello.h: 23: int alphabeta(BOARD *current,int depth,int alpha,int beta);
;P:WORTH1.C: 12: worth_1(board)
;P:WORTH1.C: 13: BOARD *board;
;P:WORTH1.C: 14: {
psect	text
global	_worth_1
_worth_1:
global	ncsv, cret, indir
call	ncsv
defw	f5
;P:WORTH1.C: 15:     static int valsum[3];
;P:WORTH1.C: 18:     register unsigned char *t;
;P:WORTH1.C: 19:     static unsigned char
;P:WORTH1.C: 20: 		val1[3]={30,0,0},
psect	data
F21:
defb	30
defb	0
defb	0
;P:WORTH1.C: 21: 		val2[3]={ 4,0,0},
F22:
defb	4
defb	0
defb	0
;P:WORTH1.C: 22: 		val3[3]={ 5,0,0};
F23:
defb	5
defb	0
defb	0
;P:WORTH1.C: 118: 	static char f[105]=
F24:
;P:WORTH1.C: 119: 				{
;P:WORTH1.C: 124: 	   0, 3, 6,  9, 3,15, 12, 18, 6,  0,45, 6,  0, 3,27, 12, 60,15,  9, 18,36,
defb	0
defb	3
defb	6
defb	9
defb	3
defb	15
defb	12
defb	18
defb	6
defb	0
defb	45
defb	6
defb	0
defb	3
defb	27
defb	12
defb	60
defb	15
defb	9
defb	18
defb	36
;P:WORTH1.C: 125: 	  21,24,27, 30,24,36, 33, 39,27, 21, 3,27, 21,24,69, 33, 18,36, 30, 39,78,
defb	21
defb	24
defb	27
defb	30
defb	24
defb	36
defb	33
defb	39
defb	27
defb	21
defb	3
defb	27
defb	21
defb	24
defb	69
defb	33
defb	18
defb	36
defb	30
defb	39
defb	78
;P:WORTH1.C: 126: 	  42,45,48, 51,45,57, 54, 60,48, 42,87,48, 42,45, 6, 54,102,57, 51, 60,15,
defb	42
defb	45
defb	48
defb	51
defb	45
defb	57
defb	54
defb	60
defb	48
defb	42
defb	87
defb	48
defb	42
defb	45
defb	6
defb	54
defb	102
defb	57
defb	51
defb	60
defb	15
;P:WORTH1.C: 127: 	  63,66,69, 72,66,78, 75, 81,69, 63,24,69, 63,66,69, 75, 39,78, 72, 81,78,
defb	63
defb	66
defb	69
defb	72
defb	66
defb	78
defb	75
defb	81
defb	69
defb	63
defb	24
defb	69
defb	63
defb	66
defb	69
defb	75
defb	39
defb	78
defb	72
defb	81
defb	78
;P:WORTH1.C: 128: 	  84,87,90, 93,87,99, 96,102,90, 84,87,90, 84,87,48, 96,102,99, 93,102,57
defb	84
defb	87
defb	90
defb	93
defb	87
defb	99
defb	96
defb	102
defb	90
defb	84
defb	87
defb	90
defb	84
defb	87
defb	48
defb	96
defb	102
defb	99
defb	93
defb	102
;P:WORTH1.C: 129: 				};
defb	57
;P:WORTH1.C: 133: 	static int v[105]=
F25:
;P:WORTH1.C: 134: 			   {
;P:WORTH1.C: 135: 		0,0,0,0,0,0,0,0,0,0,-20	,0,0,0,20	,0,-20	,0,0,0,20	,
defw	0
defw	0
defw	0
defw	0
defw	0
defw	0
defw	0
defw	0
defw	0
defw	0
defw	-20
defw	0
defw	0
defw	0
defw	20
defw	0
defw	-20
defw	0
defw	0
defw	0
defw	20
;P:WORTH1.C: 136: 		20	,20	,20	,20	,20	,20	,20	,20	,20	,20	,0,20	,20	,20	,2*20	,20	,0,20	,20	,20	,2*20	,
defw	20
defw	20
defw	20
defw	20
defw	20
defw	20
defw	20
defw	20
defw	20
defw	20
defw	0
defw	20
defw	20
defw	20
defw	40
defw	20
defw	0
defw	20
defw	20
defw	20
defw	40
;P:WORTH1.C: 137: 		-20	,-20	,-20	,-20	,-20	,-20	,-20	,-20	,-20	,-20	,-2*20	,-20	,-20	,-20	,0,-20	,
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-20
defw	-40
defw	-20
defw	-20
defw	-20
defw	0
defw	-20
;P:WORTH1.C: 138: 				-2*20	,-20	,-20	,-20	,0,
defw	-40
defw	-20
defw	-20
defw	-20
defw	0
;P:WORTH1.C: 139: 		2*20	,2*20	,2*20	,2*20	,2*20	,2*20	,2*20	,2*20	,2*20	,2*20	,20	,2*20	,2*20	,
defw	40
defw	40
defw	40
defw	40
defw	40
defw	40
defw	40
defw	40
defw	40
defw	40
defw	20
defw	40
defw	40
;P:WORTH1.C: 140: 				2*20	,2*20	,2*20	,20	,2*20	,2*20	,2*20	,2*20	,
defw	40
defw	40
defw	40
defw	20
defw	40
defw	40
defw	40
defw	40
;P:WORTH1.C: 141: 		-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
;P:WORTH1.C: 142: 				-2*20	,-2*20	,-2*20	,-20	,-2*20	,-2*20	,-2*20	,-2*20	,-2*20	,-20	
defw	-40
defw	-40
defw	-40
defw	-20
defw	-40
defw	-40
defw	-40
defw	-40
defw	-40
;P:WORTH1.C: 143: 			   };
defw	-20
;P:WORTH1.C: 149: 	*valsum=valsum[2]=0;
psect	text
ld	hl,0
ld	(F19+4),hl
ld	(F19),hl
;P:WORTH1.C: 150:     t=board->array[0];
ld	e,(ix+6)
ld	d,(ix+1+6)
ld	hl,62
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
push	bc
pop	iy
;P:WORTH1.C: 153: 	valsum[1]=
;P:WORTH1.C: 154:       v[f[f[f[f[f[f[f[  *t ]+t[ 1]]+t[ 2]]+t[ 3]]+t[ 4]]+t[ 5]]+t[ 6]]+t[ 7]];
ld	e,(iy+0)
ld	d,0
ld	hl,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+1)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+2)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+3)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+4)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+5)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+6)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+7)
ld	d,0
add	hl,de
add	hl,hl
ld	de,F25
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
ld	(F19+2),bc
;P:WORTH1.C: 155:     valsum[1]+=
;P:WORTH1.C: 156:       v[f[f[f[f[f[f[f[  *t ]+t[ 8]]+t[16]]+t[24]]+t[32]]+t[40]]+t[48]]+t[56]];
ld	e,(iy+0)
ld	d,0
ld	hl,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+8)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+16)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+24)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+32)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+40)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+48)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+56)
ld	d,0
add	hl,de
add	hl,hl
ld	de,F25
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
ld	hl,(F19+2)
add	hl,bc
ld	(F19+2),hl
;P:WORTH1.C: 157:     valsum[1]+=
;P:WORTH1.C: 158:      v[f[f[f[f[f[f[f[t[ 7]]+t[15]]+t[23]]+t[31]]+t[39]]+t[47]]+t[55]]+t[63]];
ld	e,(iy+7)
ld	d,0
ld	hl,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+15)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+23)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+31)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+39)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+47)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+55)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+63)
ld	d,0
add	hl,de
add	hl,hl
ld	de,F25
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
ld	hl,(F19+2)
add	hl,bc
ld	(F19+2),hl
;P:WORTH1.C: 159:     valsum[1]+=
;P:WORTH1.C: 160:       v[f[f[f[f[f[f[f[t[56]]+t[57]]+t[58]]+t[59]]+t[60]]+t[61]]+t[62]]+t[63]];
ld	e,(iy+56)
ld	d,0
ld	hl,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+57)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+58)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+59)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+60)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+61)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+62)
ld	d,0
add	hl,de
ld	de,F24
add	hl,de
ld	l,(hl)
ld	h,0
ld	e,(iy+63)
ld	d,0
add	hl,de
add	hl,hl
ld	de,F25
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
ld	hl,(F19+2)
add	hl,bc
ld	(F19+2),hl
;P:WORTH1.C: 175: 	valsum[(t[0])]+=50	;
ld	bc,50
ld	de,F19
ld	l,(iy+0)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 176: 	valsum[(t[1])]+=4	;
ld	de,F19
ld	l,(iy+1)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 177: 	valsum[(t[2])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+2)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 178: 	valsum[(t[3])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+3)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 179: 	valsum[(t[4])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+4)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 180: 	valsum[(t[5])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+5)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 181: 	valsum[(t[6])]+=4	;
ld	de,F19
ld	l,(iy+6)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 182: 	valsum[(t[7])]+=50	;
ld	bc,50
ld	de,F19
ld	l,(iy+7)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 184: 	valsum[(t[8])]+=4	;
ld	de,F19
ld	l,(iy+8)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 185: 	valsum[(t[9])]-=val1[*t];
ld	e,(iy+0)
ld	d,0
ld	hl,F21
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+9)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 186: 	valsum[(t[10])]-=val2[t[2]];
ld	e,(iy+2)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+10)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 187: 	valsum[(t[11])]-=val3[t[3]];
ld	e,(iy+3)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+11)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 188: 	valsum[(t[12])]-=val3[t[4]];
ld	e,(iy+4)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+12)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 189: 	valsum[(t[13])]-=val2[t[5]];
ld	e,(iy+5)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+13)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 190: 	valsum[(t[14])]-=val1[t[7]];
ld	e,(iy+7)
ld	d,0
ld	hl,F21
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+14)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 191: 	valsum[(t[15])]+=4	;
ld	de,F19
ld	l,(iy+15)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 193: 	valsum[(t[16])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+16)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 194: 	valsum[(t[17])]-=val2[t[16]];
ld	e,(iy+16)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+17)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 195: 	valsum[(t[18])]++;
ld	de,F19
ld	l,(iy+18)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 196: 	valsum[(t[21])]++;
ld	de,F19
ld	l,(iy+21)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 197: 	valsum[(t[22])]-=val2[t[23]];
ld	e,(iy+23)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+22)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 198: 	valsum[(t[23])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+23)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 200: 	valsum[(t[24])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+24)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 201: 	valsum[(t[25])]-=val3[t[24]];
ld	e,(iy+24)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+25)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 202: 	valsum[(t[30])]-=val3[t[31]];
ld	e,(iy+31)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+30)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 203: 	valsum[(t[31])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+31)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 205: 	valsum[(t[32])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+32)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 206: 	valsum[(t[33])]-=val3[t[32]];
ld	e,(iy+32)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+33)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 207: 	valsum[(t[38])]-=val3[t[39]];
ld	e,(iy+39)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+38)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 208: 	valsum[(t[39])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+39)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 210: 	valsum[(t[40])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+40)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 211: 	valsum[(t[41])]-=val2[t[40]];
ld	e,(iy+40)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+41)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 212: 	valsum[(t[42])]++;
ld	de,F19
ld	l,(iy+42)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 213: 	valsum[(t[45])]++;
ld	de,F19
ld	l,(iy+45)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 214: 	valsum[(t[46])]-=val2[t[47]];
ld	e,(iy+47)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+46)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 215: 	valsum[(t[47])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+47)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 217: 	valsum[(t[48])]+=4	;
ld	de,F19
ld	l,(iy+48)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 218: 	valsum[(t[49])]-=val1[t[56]];
ld	e,(iy+56)
ld	d,0
ld	hl,F21
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+49)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 219: 	valsum[(t[50])]-=val2[t[58]];
ld	e,(iy+58)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+50)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 220: 	valsum[(t[51])]-=val3[t[59]];
ld	e,(iy+59)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+51)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 221: 	valsum[(t[52])]-=val3[t[60]];
ld	e,(iy+60)
ld	d,0
ld	hl,F23
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+52)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 222: 	valsum[(t[53])]-=val2[t[61]];
ld	e,(iy+61)
ld	d,0
ld	hl,F22
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+53)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 223: 	valsum[(t[54])]-=val1[t[63]];
ld	e,(iy+63)
ld	d,0
ld	hl,F21
add	hl,de
ld	c,(hl)
ld	b,0
ld	de,F19
ld	l,(iy+54)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
or	a
sbc	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 224: 	valsum[(t[55])]+=4	;
ld	de,F19
ld	l,(iy+55)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 226: 	valsum[(t[56])]+=50	;
ld	bc,50
ld	de,F19
ld	l,(iy+56)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 227: 	valsum[(t[57])]+=4	;
ld	de,F19
ld	l,(iy+57)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 228: 	valsum[(t[58])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+58)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 229: 	valsum[(t[59])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+59)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 230: 	valsum[(t[60])]+=12	;
ld	bc,12
ld	de,F19
ld	l,(iy+60)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 231: 	valsum[(t[61])]+=16	;
ld	bc,16
ld	de,F19
ld	l,(iy+61)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 232: 	valsum[(t[62])]+=4	;
ld	de,F19
ld	l,(iy+62)
ld	h,0
add	hl,hl
add	hl,de
ld	c,(hl)
inc	hl
ld	b,(hl)
inc	bc
inc bc
inc bc
inc bc
ld	(hl),b
dec	hl
ld	(hl),c
;P:WORTH1.C: 233: 	valsum[(t[63])]+=50	;
ld	bc,50
ld	de,F19
ld	l,(iy+63)
ld	h,0
add	hl,hl
add	hl,de
ex	de,hl
ex	de,hl
ld	e,(hl)
inc	hl
ld	d,(hl)
ex	de,hl
add	hl,bc
ex	de,hl
ld	(hl),d
dec	hl
ld	(hl),e
ex	de,hl
;P:WORTH1.C: 235: 	return(valsum[1]-valsum[2]);
ld	de,(F19+4)
ld	hl,(F19+2)
or	a
sbc	hl,de
jp	l2
;P:WORTH1.C: 236: }
l2:
psect	bss
F19:
	defs	6
psect	text
jp	cret
f5	equ	0
,de
ld	c,(hl)
inc	hl
ld	b,