global _line_buf,_search,_raisecase,_ext_compare,_caseit,_isalpha,_isdigit,_isalnum,_isalnum0

psect text

_raisecase:
        ld      hl,0
_caseit equ $-2
        ld      a,(_ext_compare)
        ld      (ext),a
RaiseCaseTop:
        ld      a,(hl)
        or      a
        ret     z
        cp      0
ext equ $-1
        jr      z,Ext
        cp      'a'
        jr      c,DontRaise
        and     0ffh-20h
Sethl:
        ld      (hl),a
DontRaise:
        inc     hl
        jp      RaiseCaseTop
Ext:
        ld      a,'X'
        ld      (hl),a
        inc     hl
        jr      Sethl

_search:
;; byte search(char *s, byte wild)
        pop     de
        pop     hl  ; string
        pop     bc
        ld      a,c
        or      a
        push    bc
        push    hl
        push    de
        ld      de,ReallyFound
        jr      nz,skip0
        ld      de,NotWild
skip0:
        ld      ($wildchoice),de

        ld      de,_line_buf

        ld      a,(hl)
        ld      ($firstchar),a
        inc     hl
        ld      ($src),hl

skip_to_alnum:
        ld      a,(de)
        or      a
        jr      z,NotFound
        inc     de
        cp      '0'
        jr      c,skip_to_alnum
        cp      '9'+1
        jr      c,is_alnum
        cp      'A'
        jr      c,skip_to_alnum
        cp      'Z'+1
        jr      nc,skip_to_alnum

is_alnum:
        cp      0
$firstchar equ $-1
        jr      nz,skip_alnum
        ld      hl,0
$src equ $-2
compare:
        ld      a,(hl)
        or      a
        jr      z,Found
        ld      b,a
        ld      a,(de)
        cp      b
        jr      nz,skip_alnum0
        inc     hl
        inc     de
        jp      compare
Found:
        jp      ReallyFound
$wildchoice equ $-2
ReallyFound:
        ld      l,1
        ret
NotWild:
        ld      a,(de)
        cp      '0'
        jr      c,ReallyFound
        cp      '9'+1
        jr      c,skip_alnum1
        cp      'A'
        jr      c,ReallyFound
        cp      'Z'+1
        jr      nc,ReallyFound
skip_alnum:
        ld      a,(de)
skip_alnum0:
        or      a
        jr      z,NotFound
skip_alnum1:
        inc     de
        cp      '0'
        jr      c,skip_to_alnum
        cp      '9'+20h
        jr      nc,skip_alnum
        cp      'A'
        jr      c,skip_to_alnum
        cp      'Z'+1
        jr      nc,skip_to_alnum
        jp      skip_alnum

NotFound:
        ld      l,0
        ret

global _isalnum,_isalpha,_isdigit
_isdigit:
        pop     bc
        pop     hl
        push    hl
        push    bc
isdigit:
        ld      a,l
        ld      hl,0
        cp      '0'
        ret     c
        cp      '9'+1
        ret     nc
        inc     hl
        ret

_isalpha:
        pop     bc
        pop     hl
        push    hl
        push    bc
        ld      a,l
        ld      hl,0
        or      20h
        cp      'a'
        ret     c
        cp      'z'+1
        ret     nc
        inc     hl
        ret

_isalnum0:
        pop     bc
        pop     hl
        push    hl
        push    bc
        ld      a,l
        ld      l,1  ;; h is irrelevant for nonzero return
        cp      ' '
        ret     z
        cp      '.'
        ret     z
        cp      '-'
        ret     z
_isalnum:
        pop     bc
        pop     hl
        push    hl
        push    bc
isalnum:
        push    hl
        call    _isalpha
        pop     bc
        ld      a,l
        or      a
        ret     nz
        push    bc
        pop     hl
        jr      isdigit






















































