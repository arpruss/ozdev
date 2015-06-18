global _ozpoint,lmul,ldiv,_ozcircle,_ozline

psect data   ;; self-mod code

pointonly:
        ld      c,(ix+8)  ; c
        push    bc
        ld      c,(ix+4)  ; cy
        ld      b,(ix+5)
        push    bc
        ld      c,(ix+2)  ; cx
        ld      b,(ix+3)
        push    bc
        call    _ozpoint
        pop     af
        pop     af
        pop     af
        pop     ix
        ret

Small:
        pop     ix
global __ozcirc25
        jp      __ozcirc25

_ozcircle:  ;; ozcircle(int cx, int cy, byte r, byte c)
        push    ix
        ld      ix,2
        add     ix,sp
        ld      a,(ix+6)       ; r
        or      a
        jp      z,pointonly

        ld      l,(ix+2)
        ld      h,(ix+3)
        ld      (cx),hl

        dec     a
        ld      (didy),a   ; didy=0ffh

        ld      l,(ix+4)
        ld      h,(ix+5)
        ld      (cy),hl

        ld      a,(ix+6)
        cp      1
        jp      z,FourCornersF

        cp      6
        jr      c,Small

        ld      l,a
        ld      h,0        ; hl=r
        ld      de,181     ; 256/sqrt(2)
        call    lmul       ; hl=r/sqrt(2)*256
        ld      a,h
        or      a
        jp      z,FourCornersF
        inc     h
TheLoop:
        ld      l,0        ; hl=256*x
        push    hl
        ld      e,(ix+6)
        ld      d,l
        call    ldiv       ; hl=x*256/r
        ld      de,_table
        add     hl,de
        ld      l,(hl)
        ld      h,0
        ld      e,(ix+6)
        ld      d,h
        call    lmul       ; hl=table[i*256/r]*r
        ld      de,127
        add     hl,de
        ld      c,h
        ld      b,0        ; bc=y
        ld      a,c
        ld      (y),a
        xor     a
        ld      (y+1),a
        pop     hl
        ld      e,h
        ld      d,b        ; de=x
        ld      (x+1),a
        ld      a,e
        ld      (x),a
        ld      l,(ix+8)   ; c
        push    hl

        bit     2,l
        jp      nz,FilledCircle
        cp      c
        jp      z,LastFour
        jp      nc,Next

        ld      hl,0000    ;; self modifying code: cy put in 0000
cy      equ     $-2
        add     hl,bc
        push    hl         ; cy+y
        ld      hl,0000    ;; self modifying code: cx put in 0000
cx      equ     $-2
        add     hl,de
        push    hl         ; cx+x
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        or      a
        ld      bc,0000    ;; self modifying code: y put in 0000
y       equ     $-2
        sbc     hl,bc
        push    hl         ; cy-y
        ld      hl,(cx)
        ld      de,0000
x       equ     $-2        ;; self modifying code; x put in 0000
        add     hl,de
        push    hl         ; cx+x
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        or      a
        ld      bc,(y)
        sbc     hl,bc
        push    hl         ; cy-y
        ld      hl,(cx)
        ld      de,(x)
        or      a
        sbc     hl,de
        push    hl         ; cx-x
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        ld      bc,(y)
        add     hl,bc
        push    hl         ; cy+y
        ld      hl,(cx)
        ld      de,(x)
        or      a
        sbc     hl,de
        push    hl         ; cx-x
        call    _ozpoint
        pop     af
        pop     af

LastFour:
        ld      hl,(cy)
        ld      bc,(x)
        add     hl,bc
        push    hl         ; cy+x
        ld      hl,(cx)
        ld      de,(y)
        add     hl,de
        push    hl         ; cx+y
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        or      a
        ld      bc,(x)
        sbc     hl,bc
        push    hl         ; cy-x
        ld      hl,(cx)
        ld      de,(y)
        add     hl,de
        push    hl         ; cx+y
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        or      a
        ld      bc,(x)
        sbc     hl,bc
        push    hl         ; cy-y
        ld      hl,(cx)
        ld      de,(y)
        or      a
        sbc     hl,de
        push    hl         ; cx-x
        call    _ozpoint
        pop     af
        pop     af

        ld      hl,(cy)
        ld      bc,(x)
        add     hl,bc
        push    hl         ; cy+y
        ld      hl,(cx)
        ld      de,(y)
        or      a
        sbc     hl,de
        push    hl         ; cx-x
        call    _ozpoint
ThreePops:
        pop     af
        pop     af

Next:
        pop     af

        ld      a,(x)
        ld      h,a
        dec     h
        jp      nz,TheLoop
FourCorners:
        ld      l,(ix+8)   ; c
        push    hl
        bit     2,l
        jp      nz,FilledCorners
FourCornersDO:
        ld      hl,(cy)
        ld      c,(ix+6)   ; r
        ld      b,0
        add     hl,bc
        push    hl         ; cy+r
        ld      hl,(cx)
        push    hl         ; cx
        call    _ozpoint
        pop     af
        pop     af
        ld      hl,(cy)
        xor     a
        ld      c,(ix+6)
        ld      b,a
        sbc     hl,bc     ; cy-r
        push    hl
        ld      hl,(cx)
        push    hl
        call    _ozpoint
        pop     af
        pop     af
        ld      hl,(cy)
        push    hl
        ld      hl,(cx)
        ld      c,(ix+6)
        ld      b,0
        add     hl,bc
        push    hl
        call    _ozpoint
        pop     af
        ld      hl,(cx)
        ld      c,(ix+6)
        xor     a
        ld      b,a
        sbc     hl,bc
        push    hl
        call    _ozpoint
Pop3Out:
        pop     af
        pop     af
Pop1Out:
        pop     af
GetOut:
        pop     ix
        ret
FourCornersF:
        ld      c,(ix+8)      ; c
        push    bc
        bit     2,c
        jp      z,FourCornersDO
        ld      c,(ix+4)  ; cy
        push    bc
        ld      c,(ix+2)  ; cx
        push    bc
        call    _ozpoint
        pop     bc
        pop     bc
        jp      FourCornersDO
FilledCorners:
        ld      c,(ix+6)      ; r
        ld      b,0
        ld      hl,(cy)
        add     hl,bc
        push    hl            ; y2
        ld      de,(cx)
        push    de            ; x2
        ld      hl,(cy)
        or      a
        sbc     hl,bc
        push    hl            ; y1
        push    de            ; x1

        call    _ozline
        pop     af
        pop     af
        pop     af
        pop     af

        ld      a,(didy)
        ld      c,(ix+6)      ; r
        cp      c
        jr      z,Pop1Out
        ld      b,0
        ld      hl,(cy)
        push    hl
        ld      hl,(cx)
        add     hl,bc
        push    hl
        call    _ozpoint
        pop     af
        ld      hl,(cx)
        or      a
        ld      c,(ix+6)
        ld      b,0
        sbc     hl,bc
        push    hl
        call    _ozpoint
        jp      Pop3Out
FilledCircle:
        cp      c
        jp      z,ZZLastTwo
        jp      nc,Next
        ld      hl,(cy)
        add     hl,bc
        push    hl              ; y2
        ld      hl,(cx)
        add     hl,de
        ex      de,hl
        push    de              ; x
        ld      hl,(cy)
        sbc     hl,bc
        push    hl              ; y1
        push    de              ; x
        call    _ozline
        pop     af
        pop     bc              ; y1
        pop     af

        ld      hl,(cx)
        ld      de,(x)
        or      a
        sbc     hl,de
        push    hl              ;
        push    bc
        push    hl
        call    _ozline
        pop     af
        pop     af
        pop     af
        pop     af
LastTwo:
        ld      a,(y)
        cp      00        ;; self-modifying code: didy
didy    equ     $-1
        jp      z,Next
ZZLastTwo:
        ld      (didy),a
        ld      bc,(x)
        ld      hl,(cy)
        add     hl,bc
        push    hl              ; y2
        ld      hl,(cx)
        ld      de,(y)
        add     hl,de
        ex      de,hl
        push    de              ; x2
        ld      hl,(cy)
        or      a
        sbc     hl,bc
        push    hl              ; y1
        push    de              ; x2
        call    _ozline
        pop     af
        pop     de
        pop     af
        ld      hl,(cx)
        ld      bc,(y)
        or      a
        sbc     hl,bc
        push    hl              ; x2
        push    de              ; y1
        push    hl              ; x1
        call    _ozline
        pop     af
        pop     af
        jp      ThreePops

psect   data
_table:
;defb 255 ; (0)
defb 255 ; (1)
defb 255 ; (2)
defb 255 ; (3)
defb 255 ; (4)
defb 255 ; (5)
defb 255 ; (6)
defb 255 ; (7)
defb 255 ; (8)
defb 255 ; (9)
defb 255 ; (10)
defb 255 ; (11)
defb 255 ; (12)
defb 255 ; (13)
defb 255 ; (14)
defb 255 ; (15)
defb 255 ; (16)
defb 254 ; (17)
defb 254 ; (18)
defb 254 ; (19)
defb 254 ; (20)
defb 254 ; (21)
defb 254 ; (22)
defb 254 ; (23)
defb 254 ; (24)
defb 254 ; (25)
defb 254 ; (26)
defb 254 ; (27)
defb 253 ; (28)
defb 253 ; (29)
defb 253 ; (30)
defb 253 ; (31)
defb 253 ; (32)
defb 253 ; (33)
defb 253 ; (34)
defb 253 ; (35)
defb 252 ; (36)
defb 252 ; (37)
defb 252 ; (38)
defb 252 ; (39)
defb 252 ; (40)
defb 252 ; (41)
defb 252 ; (42)
defb 251 ; (43)
defb 251 ; (44)
defb 251 ; (45)
defb 251 ; (46)
defb 251 ; (47)
defb 250 ; (48)
defb 250 ; (49)
defb 250 ; (50)
defb 250 ; (51)
defb 250 ; (52)
defb 249 ; (53)
defb 249 ; (54)
defb 249 ; (55)
defb 249 ; (56)
defb 249 ; (57)
defb 248 ; (58)
defb 248 ; (59)
defb 248 ; (60)
defb 248 ; (61)
defb 247 ; (62)
defb 247 ; (63)
defb 247 ; (64)
defb 247 ; (65)
defb 246 ; (66)
defb 246 ; (67)
defb 246 ; (68)
defb 246 ; (69)
defb 245 ; (70)
defb 245 ; (71)
defb 245 ; (72)
defb 244 ; (73)
defb 244 ; (74)
defb 244 ; (75)
defb 244 ; (76)
defb 243 ; (77)
defb 243 ; (78)
defb 243 ; (79)
defb 242 ; (80)
defb 242 ; (81)
defb 242 ; (82)
defb 241 ; (83)
defb 241 ; (84)
defb 241 ; (85)
defb 240 ; (86)
defb 240 ; (87)
defb 239 ; (88)
defb 239 ; (89)
defb 239 ; (90)
defb 238 ; (91)
defb 238 ; (92)
defb 238 ; (93)
defb 237 ; (94)
defb 237 ; (95)
defb 236 ; (96)
defb 236 ; (97)
defb 236 ; (98)
defb 235 ; (99)
defb 235 ; (100)
defb 234 ; (101)
defb 234 ; (102)
defb 233 ; (103)
defb 233 ; (104)
defb 233 ; (105)
defb 232 ; (106)
defb 232 ; (107)
defb 231 ; (108)
defb 231 ; (109)
defb 230 ; (110)
defb 230 ; (111)
defb 229 ; (112)
defb 229 ; (113)
defb 228 ; (114)
defb 228 ; (115)
defb 227 ; (116)
defb 227 ; (117)
defb 226 ; (118)
defb 226 ; (119)
defb 225 ; (120)
defb 225 ; (121)
defb 224 ; (122)
defb 224 ; (123)
defb 223 ; (124)
defb 223 ; (125)
defb 222 ; (126)
defb 221 ; (127)
defb 221 ; (128)
defb 220 ; (129)
defb 220 ; (130)
defb 219 ; (131)
defb 218 ; (132)
defb 218 ; (133)
defb 217 ; (134)
defb 217 ; (135)
defb 216 ; (136)
defb 215 ; (137)
defb 215 ; (138)
defb 214 ; (139)
defb 213 ; (140)
defb 213 ; (141)
defb 212 ; (142)
defb 212 ; (143)
defb 211 ; (144)
defb 210 ; (145)
defb 209 ; (146)
defb 209 ; (147)
defb 208 ; (148)
defb 207 ; (149)
defb 207 ; (150)
defb 206 ; (151)
defb 205 ; (152)
defb 204 ; (153)
defb 204 ; (154)
defb 203 ; (155)
defb 202 ; (156)
defb 201 ; (157)
defb 201 ; (158)
defb 200 ; (159)
defb 199 ; (160)
defb 198 ; (161)
defb 197 ; (162)
defb 197 ; (163)
defb 196 ; (164)
defb 195 ; (165)
defb 194 ; (166)
defb 193 ; (167)
defb 192 ; (168)
defb 192 ; (169)
defb 191 ; (170)
defb 190 ; (171)
defb 189 ; (172)
defb 188 ; (173)
defb 187 ; (174)
defb 186 ; (175)
defb 185 ; (176)
defb 184 ; (177)
defb 183 ; (178)
defb 182 ; (179)
defb 181 ; (180)
defb 180 ; (181)
defb 179 ; (182)
defb 178 ; (183)
defb 177 ; (184)
defb 176 ; (185)
defb 175 ; (186)
defb 174 ; (187)
defb 173 ; (188)
defb 172 ; (189)
defb 171 ; (190)
defb 170 ; (191)
defb 169 ; (192)
defb 168 ; (193)
defb 166 ; (194)
defb 165 ; (195)
defb 164 ; (196)
defb 163 ; (197)
defb 162 ; (198)
defb 160 ; (199)
defb 159 ; (200)
defb 158 ; (201)
defb 157 ; (202)
defb 155 ; (203)
defb 154 ; (204)
defb 153 ; (205)
defb 151 ; (206)
defb 150 ; (207)
defb 149 ; (208)
defb 147 ; (209)
defb 146 ; (210)
defb 144 ; (211)
defb 143 ; (212)
defb 141 ; (213)
defb 140 ; (214)
defb 138 ; (215)
defb 137 ; (216)
defb 135 ; (217)
defb 134 ; (218)
defb 132 ; (219)
defb 130 ; (220)
defb 129 ; (221)
defb 127 ; (222)
defb 125 ; (223)
defb 123 ; (224)
defb 122 ; (225)
defb 120 ; (226)
defb 118 ; (227)
defb 116 ; (228)
defb 114 ; (229)
defb 112 ; (230)
defb 110 ; (231)
defb 108 ; (232)
defb 106 ; (233)
defb 103 ; (234)
defb 101 ; (235)
defb 99 ; (236)
defb 96 ; (237)
defb 94 ; (238)
defb 91 ; (239)
defb 89 ; (240)
defb 86 ; (241)
defb 83 ; (242)
defb 80 ; (243)
defb 77 ; (244)
defb 74 ; (245)
defb 71 ; (246)
defb 67 ; (247)
defb 63 ; (248)
defb 59 ; (249)
defb 55 ; (250)
defb 50 ; (251)
defb 45 ; (252)
defb 39 ; (253)
defb 32 ; (254)
defb 23 ; (255)
