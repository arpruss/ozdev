WIDTH   equ 29

        psect text
global _beep
_beep:
        xor     a
	out	(19h),a
	ld	a,20h
	out	(15h),a
	ret

        psect lowpage

global _ttyscroll
_ttyscroll:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,0400h
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      de,0a001h

        ld      hl,30*8
        add     hl,de
        ld      bc,2400-8*30
        ldir
        ld      l,e
        ld      h,d
        inc     de
        ld      bc,30*8-1
	ld	(hl),0
        ldir

scroll_cleanup:
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret


_curcol:
        defb 0
_currow:
        defb 0

global _xorcursor,_curcol,_currow
_xorcursor:
        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc
        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

        ld      a,(_curcol) ; col
        ld      c,a
        ld      a,(_currow) ; row
                            ; calculate screen offset, result in hl
	sla	a
	sla	a
	sla	a
	ld	l,a
        ld      h,0       ; hl=8*row
        add     hl,hl     ; hl=2*8*row
        ld      e,l
        ld      d,h       ; de=2*8*row
        add     hl,hl     ; hl=4*8*row
        add     hl,hl     ; hl=8*8*row
        add     hl,hl     ; hl=16*8*row
        add     hl,hl     ; hl=32*8*row
        sbc     hl,de     ; hl=30*8*row
        ld      de,7*30
        add     hl,de
        ld      b,0a0h
        add     hl,bc
        ld      a,(hl)
        xor     7fh
        ld      (hl),a
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret

global _ttyprint
; ttyprint(char c)
_ttyprint:
        ld      a,(_curcol) ; col
        cp      WIDTH
global  _crlf
        call    nc,_crlf

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        xor     a
        out     (3),a
        ld      a,4
        out     (4),a

        push    ix

        ld      hl,6
        add     hl,sp
        ld      l,(hl)          ; print char
        ld      h,0

        add     hl,hl  ; hl=char*2   ;11
        add     hl,hl  ; hl=char*4
        add     hl,hl  ; hl=char*8
        ex      de,hl                ;4
        ld      ix,font              ;14
        add     ix,de                ;15  ;; ix=font+char*8

        ld      hl,_curcol
        ld      c,(hl)
        inc     hl
        ld      a,(hl) ; _currow
        sla     a
	sla	a
	sla	a
	ld	l,a
        ld      h,0       ; hl=8*row
        add     hl,hl     ; hl=2*8*row
        ld      e,l
        ld      d,h       ; de=2*8*row
        add     hl,hl     ; hl=4*8*row
        add     hl,hl     ; hl=8*8*row
        add     hl,hl     ; hl=16*8*row
        add     hl,hl     ; hl=32*8*row
        sbc     hl,de     ; hl=30*8*row
        ld      b,0a0h
        add     hl,bc
        ld      bc,30

char0:
        ld      a,(ix)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+1)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+2)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+3)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+4)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+5)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+6)
        ld      (hl),a
        add     hl,bc
        ld      a,(ix+7)
        ld      (hl),a
        inc     hl
        ld      a,(hl)
        xor     7fh
        ld      (hl),a
cursor:
        ld      hl,_curcol
        inc     (hl)

        pop     ix
        pop     bc

        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret


font:
defb  0,0,0,0,0,0,0,0  ; 0
defb  0,0,0,8,0,0,0,0  ; 250
defb  0,28,62,62,62,28,0,0  ; 254
defb  0,28,34,34,34,28,0,0    ; ((3))
;;defb  99,119,127,127,62,28,8,0  ; 3
defb  8,28,62,127,62,28,8,0  ; 4
defb  8,28,8,119,119,8,28,0  ; 5
defb  28,62,127,127,54,8,28,0  ; 6
defb  28,62,127,127,54,8,28,0  ; 7
defb  255,231,195,129,195,231,255,255  ; 8
defb  0,0,0,0,0,0,0,0  ; 9
defb  0,0,0,0,0,0,0,0  ; 10
defb  0,0,0,0,0,0,0,0  ; 11
defb  0,0,0,0,0,0,0,0  ; 12
defb  0,0,0,0,0,0,0,0  ; 13
defb  124,124,108,108,111,102,120,48  ; 14
defb  219,60,102,231,102,60,219,0  ; 15
defb  1,3,15,31,15,3,1,0  ; 16
defb  64,96,120,124,120,96,64,0  ; 17
defb  24,60,126,24,126,60,24,0  ; 18
defb  102,102,102,102,102,0,102,0  ; 19
defb  254,219,222,220,216,216,216,0  ; 20
defb  60,102,28,54,54,28,51,30  ; 21
defb  0,0,0,0,127,127,127,0  ; 22
defb  24,60,126,24,126,60,24,126  ; 23
defb  24,60,126,24,24,24,24,0  ; 24
defb  24,24,24,24,126,60,24,0  ; 25
defb  0,24,56,127,56,24,0,0  ; 26
defb  0,12,14,127,14,12,0,0  ; 27
defb  0,12,14,127,14,12,0,0  ; 28
defb  0,12,14,127,14,12,0,0  ; 29
defb  0,12,14,127,14,12,0,0  ; 30
defb  0,12,14,127,14,12,0,0  ; 31
defb  0,0,0,0,0,0,0,0  ; 32
defb  24,60,60,24,24,0,24,0  ; 33
defb  54,54,54,0,0,0,0,0  ; 34
defb  54,54,127,54,127,54,54,0  ; 35
defb  24,126,3,62,96,63,24,0  ; 36
defb  0,99,48,24,12,6,99,0  ; 37
defb  28,54,28,110,51,51,110,0  ; 38
defb  24,24,12,0,0,0,0,0  ; 39
defb  24,12,6,6,6,12,24,0  ; 40
defb  6,12,24,24,24,12,6,0  ; 41
defb  0,119,62,127,62,119,0,0  ; 42
defb  0,24,24,126,24,24,0,0  ; 43
defb  0,0,0,0,24,24,12,0  ; 44
defb  0,0,0,127,0,0,0,0  ; 45
defb  0,0,0,0,0,28,28,0  ; 46
defb  96,48,24,12,6,3,1,0  ; 47
defb  62,99,115,123,111,103,62,0  ; 48
defb  24,30,24,24,24,24,126,0  ; 49
defb  62,99,48,24,12,102,127,0  ; 50
defb  62,99,96,60,96,99,62,0  ; 51
defb  48,56,60,54,127,48,48,0  ; 52
defb  127,3,63,96,96,99,62,0  ; 53
defb  62,99,3,63,99,99,62,0  ; 54
defb  127,99,96,48,24,24,24,0  ; 55
defb  62,99,99,62,99,99,62,0  ; 56
defb  62,99,99,126,96,99,62,0  ; 57
defb  0,56,56,0,0,56,56,0  ; 58
defb  0,24,24,0,0,24,24,12  ; 59
defb  48,24,12,6,12,24,48,0  ; 60
defb  0,0,127,0,0,127,0,0  ; 61
defb  6,12,24,48,24,12,6,0  ; 62
defb  62,99,96,48,24,0,24,0  ; 63
defb  62,99,99,123,59,3,126,0  ; 64
defb  28,54,99,99,127,99,99,0  ; 65
defb  63,102,102,62,102,102,63,0  ; 66
defb  60,102,3,3,3,102,60,0  ; 67
defb  31,54,102,102,102,54,31,0  ; 68
defb  127,67,3,31,3,67,127,0  ; 69
defb  127,70,6,62,6,6,15,0  ; 70
defb  62,99,3,3,123,99,62,0  ; 71
defb  99,99,99,127,99,99,99,0  ; 72
defb  60,24,24,24,24,24,60,0  ; 73
defb  60,24,24,24,27,27,14,0  ; 74
defb  99,51,27,15,27,51,99,0  ; 75
defb  15,6,6,6,6,70,127,0  ; 76
defb  99,119,127,107,107,99,99,0  ; 77
defb  99,103,103,111,123,115,99,0  ; 78
defb  62,99,99,99,99,99,62,0  ; 79
defb  63,102,102,62,6,6,15,0  ; 80
defb  62,99,99,99,99,107,62,96  ; 81
defb  63,99,99,63,27,51,99,0  ; 82
defb  62,99,3,62,96,99,62,0  ; 83
defb  126,90,24,24,24,24,60,0  ; 84
defb  99,99,99,99,99,99,62,0  ; 85
defb  99,99,99,99,54,28,8,0  ; 86
defb  99,99,107,107,127,119,99,0  ; 87
defb  99,54,28,28,28,54,99,0  ; 88
defb  102,102,102,60,24,24,60,0  ; 89
defb  127,97,48,24,12,70,127,0  ; 90
defb  62,6,6,6,6,6,62,0  ; 91
defb  3,6,12,24,48,96,64,0  ; 92
defb  62,48,48,48,48,48,62,0  ; 93
defb  8,28,54,99,0,0,0,0  ; 94
defb  0,0,0,0,0,0,0,255  ; 95
defb  12,12,24,0,0,0,0,0  ; 96
defb  0,0,30,48,62,51,126,0  ; 97
defb  7,6,62,102,102,102,63,0  ; 98
defb  0,0,62,99,3,99,62,0  ; 99
defb  56,48,62,51,51,51,126,0  ; 100
defb  0,0,62,99,127,3,62,0  ; 101
defb  56,108,12,63,12,12,30,0  ; 102
defb  0,0,110,115,99,126,96,62  ; 103
defb  7,6,62,102,102,102,103,0  ; 104
defb  24,0,28,24,24,24,60,0  ; 105
defb  48,0,56,48,48,48,51,30  ; 106
defb  7,6,102,54,30,54,103,0  ; 107
defb  24,24,24,24,24,24,56,0  ; 108
defb  0,0,54,127,107,107,99,0  ; 109
defb  0,0,59,102,102,102,102,0  ; 110
defb  0,0,62,99,99,99,62,0  ; 111
defb  0,0,59,102,102,62,6,15  ; 112
defb  0,0,110,51,51,62,48,120  ; 113
defb  0,0,59,102,6,6,15,0  ; 114
defb  0,0,62,3,62,96,62,0  ; 115
defb  12,12,63,12,12,108,56,0  ; 116
defb  0,0,51,51,51,51,110,0  ; 117
defb  0,0,99,99,54,28,8,0  ; 118
defb  0,0,99,99,107,127,54,0  ; 119
defb  0,0,99,54,28,54,99,0  ; 120
defb  0,0,99,99,115,110,96,62  ; 121
defb  0,0,63,25,12,38,63,0  ; 122
defb  112,24,24,14,24,24,112,0  ; 123
defb  24,24,24,0,24,24,24,0  ; 124
defb  14,24,24,112,24,24,14,0  ; 125
defb  110,59,0,0,0,0,0,0  ; 126
defb  0,8,28,28,54,54,127,0  ; 127

