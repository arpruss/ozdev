psect   lowpage

_curcol:
        defb 0
_currow:
        defb 0
__60spaces:
        defm '                                                            '
        defb 0

global _curcol,_currow,__60spaces

global _ttyprintstring
; ttyprint(char *s)
_ttyprintstring:
        exx
        pop     de
        pop     hl
        push    hl
        push    de
        exx

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc
        push    ix

        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

Top:
        exx
        ld      a,(hl)
        inc     hl
        exx
        or      a
        jp      z,Done

        ld      l,a          ; print char
        ld      h,0

        add     hl,hl  ; hl=char*2   ;11
        ld      c,l                  ;4
        ld      b,h                  ;4
        add     hl,bc  ; hl=char*4   ;11
        add     hl,bc  ; hl=char*6   ;11
        ex      de,hl                ;4
        ld      ix,font              ;14
        add     ix,de                ;15  ;; ix=font+char*6

        ld      hl,_curcol
        ld      c,(hl)
        inc     hl
        ld      a,(hl) ; _currow
        ld      b,a
                     ; calculate screen offset, result in hl
	sla	a
	sla	a
	sla	a
	sla	a
        sub     b
	ld	l,a
	ld	h,0
	ld	d,h
	ld	e,l
	add	hl,hl
	add	hl,de
	add	hl,hl
	add	hl,hl
        ld      d,0a0h
        ld      e,c
	srl	e
	add	hl,de

        bit     0,c
        ld      bc,30
        jp      nz,char1

char0:  ld      e,0f0h
        ld      a,e          ; big unrolled loop for even chars
	and	(hl)
	or	(ix)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+1)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+2)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+3)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+4)
	ld	(hl),a
        add     hl,bc
        ld      a,e
	and	(hl)
        or      (ix+5)
        ld      (hl),a

	jp	cursor

char1:	ld	a,(ix)		; big unrolled loop for odd chars
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      d,0fh
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+1)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+2)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+3)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+4)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
	add	a,e
	ld	(hl),a
        add     hl,bc
        ld      a,(ix+5)
	add	a,a
	add	a,a
	add	a,a
	add	a,a
	ld	e,a
        ld      a,d
	and	(hl)
        add     a,e
        ld      (hl),a

cursor:
        ld      hl,_curcol
        inc     (hl)
        jp      Top

Done:
        pop     ix
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret


font:
defb 0,0,0,0,0,0;(0=.)
defb 0,5,0,5,2,0;(1=.)
defb 0,5,0,0,7,0;(2=.)
defb 0,5,7,7,2,0;(3=.)
defb 0,2,7,7,2,0;(4=.)
defb 6,15,15,6,15,0;(5=.)
defb 2,7,2,2,7,0;(6=.)
defb 0,0,0,0,0,0;(7=.)
defb 15,15,9,9,15,15;(8=.)
defb 0,0,0,0,0,0;(9=.)
defb 0,0,0,0,0,0;(10=.)
defb 0,0,0,0,0,0;(11=.)
defb 0,0,0,0,0,0;(12=.)
;; defb 0,0,0,0,0,0;(13=.)
defb 0000B
defb 1000B
defb 1000B
defb 1010B
defb 1111B
defb 0010B


defb 14,10,10,10,5,0;(14=.)
defb 9,6,9,9,6,9;(15=.)
defb 1,3,7,3,1,0;(16=.)
defb 4,6,7,6,4,0;(17=.)
defb 2,7,2,2,7,2;(18=.)
defb 5,5,5,0,5,0;(19=.)
defb 14,11,11,10,10,0;(20=.)
defb 14,1,6,6,8,7;(21=.)
defb 0,0,0,15,15,0;(22=.)
defb 2,7,2,7,2,15;(23=.)
defb 2,7,2,2,2,2;(24=.)
defb 2,2,2,2,7,2;(25=.)
defb 0,4,15,4,0,0;(26=.)
defb 0,2,15,2,0,0;(27=.)
defb 0,0,0,0,0,0;(28=.)
defb 0,0,0,0,0,0;(29=.)
defb 0,0,0,0,0,0;(30=.)
defb 0,0,0,0,0,0;(31=.)
defb 0,0,0,0,0,0;(32= )
defb 1,1,1,0,1,0;(33=!)
defb 5,5,0,0,0,0;(34=")
defb 6,15,6,15,6,0;(35=#)
defb 6,3,7,6,3,0;(36=$)
defb 1,4,2,1,4,0;(37=%)
defb 2,5,2,5,10,0;(38=&)
defb 2,2,1,0,0,0;(39=')
defb 4,2,2,2,4,0;(40=()
defb 1,2,2,2,1,0;(41=))
defb 5,2,7,2,5,0;(42=*)
defb 0,2,7,2,0,0;(43=+)
defb 0,0,0,2,1,0;(44=,)
defb 0,0,7,0,0,0;(45=-)
defb 0,0,0,0,1,0;(46=.)
defb 4,2,2,2,1,0;(47=/)
;defb 2,5,5+2,5,2,0;(48=0)
defb 2,5+2,5,5,2,0;(48=0)
defb 2,3,2,2,2,0;(49=1)
defb 3,4,2,1,7,0;(50=2)
defb 3,4,3,4,3,0;(51=3)
defb 4,6,5,7,4,0;(52=4)
defb 7,1,3,4,3,0;(53=5)
defb 6,1,3,5,2,0;(54=6)
defb 7,4,2,2,2,0;(55=7)
defb 2,5,2,5,2,0;(56=8)
defb 2,5,6,4,2,0;(57=9)
defb 0,2,0,2,0,0;(58=:)
defb 0,2,0,2,1,0;(59=;)
defb 4,2,1,2,4,0;(60=<)
defb 0,7,0,7,0,0;(61==)
defb 1,2,4,2,1,0;(62=>)
defb 3,4,2,0,2,0;(63=?)
defb 6,9,13,13,1,6;(64=@)
defb 2,5,7,5,5,0;(65=A)
defb 3,5,3,5,3,0;(66=B)
defb 6,1,1,1,6,0;(67=C)
defb 3,5,5,5,3,0;(68=D)
defb 7,1,3,1,7,0;(69=E)
defb 7,1,3,1,1,0;(70=F)
defb 6,1,5,5,6,0;(71=G)
defb 5,5,7,5,5,0;(72=H)
defb 7,2,2,2,7,0;(73=I)
defb 7,2,2,2,1,0;(74=J)
defb 5,5,3,5,5,0;(75=K)
defb 1,1,1,1,7,0;(76=L)
defb 5,7,7,5,5,0;(77=M)
defb 3,5,5,5,5,0;(78=N)
defb 2,5,5,5,2,0;(79=O)
defb 3,5,3,1,1,0;(80=P)
defb 2,5,5,3,6,0;(81=Q)
defb 3,5,3,5,5,0;(82=R)
defb 6,1,2,4,3,0;(83=S)
defb 7,2,2,2,2,0;(84=T)
defb 5,5,5,5,6,0;(85=U)
defb 5,5,5,5,2,0;(86=V)
defb 5,5,7,7,5,0;(87=W)
defb 5,5,2,5,5,0;(88=X)
defb 5,5,2,2,2,0;(89=Y)
defb 7,4,2,1,7,0;(90=Z)
defb 6,2,2,2,6,0;(91=[)
defb 1,2,2,2,4,0;(92=\)
defb 3,2,2,2,3,0;(93=])
defb 2,5,0,0,0,0;(94=^)
defb 0,0,0,0,15,0;(95=_)
;_byte_0_15 equ $-2
;global _byte_0_15
defb 2,2,4,0,0,0;(96=`)
defb 0,3,6,5,6,0;(97=a)
defb 1,3,5,5,3,0;(98=b)
defb 0,6,1,1,6,0;(99=c)
defb 4,6,5,5,6,0;(100=d)
defb 0,2,5,3,6,0;(101=e)
defb 4,2,7,2,2,0;(102=f)
defb 0,6,5,6,3,0;(103=g)
defb 1,3,5,5,5,0;(104=h)
defb 2,0,2,2,2,0;(105=i)
defb 4,0,4,4,2,0;(106=j)
defb 1,1,5,3,5,0;(107=k)
defb 2,2,2,2,4,0;(108=l)
defb 0,3,7,7,5,0;(109=m)
defb 0,3,5,5,5,0;(110=n)
defb 0,2,5,5,2,0;(111=o)
defb 0,3,5,3,1,0;(112=p)
defb 0,6,5,6,12,0;(113=q)
defb 0,3,5,1,1,0;(114=r)
defb 0,6,3,6,3,0;(115=s)
defb 2,7,2,2,4,0;(116=t)
defb 0,5,5,5,6,0;(117=u)
defb 0,5,5,5,2,0;(118=v)
defb 0,5,7,7,2,0;(119=w)
defb 0,5,2,2,5,0;(120=x)
defb 0,5,5,6,3,0;(121=y)
defb 0,7,4,2,7,0;(122=z)
defb 4,2,3,2,4,0;(123={)
defb 2,2,0,2,2,0;(124=|)
defb 1,2,6,2,1,0;(125=})
defb 10,5,0,0,0,0;(126=~)
defb 0,2,5,7,0,0;(127=.)

