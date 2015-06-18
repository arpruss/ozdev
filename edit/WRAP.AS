psect beforebss
global _line_buf
global _iloc_and_wt,_file_length,_clear_wrap_to_end
global _wrap_line,_grab_line,_find_next_line,_find_prev_line
global _wrapped_length,_insert,_delete
global $aux_pagein,$aux_pageout
global _cleararea,__ozactivepage
global _search,_export_file
global _search_string,_import_file,_xlat_table
global _do_grab_transl

_width_table equ _iloc_and_wt

SCREEN_WIDTH equ 239

_xlat_table:
defb 233,243,250,225,232,242,249,224,186,183,209,181,128,13,234,244,251,226
defb 202,212,235,246,252,228,203,214,220,196,223,231,163,241

range181_255:
defb 11,255,9,255,255,8,255,255,255,255,255,255,255,255,255,27,255
defb 255,255,255,255,18,24,255,255,255,255,255,10,255,255,19,255,25
defb 255,255,255,255,255,26,255,255,28,7,3,17,255,23,255,255,29,4
defb 0,14,20,255,255,255,255,255,31,5,1,15,255,21,255,255,6,2,16
defb 22,255,255,255

_line_buf:
defs 120
defb 0,0,0,0
_file_length: defw 0
_wrapped_length: defw 0
start: defw 0
_search_string:
searchstring: defb 0
              defs 40

_search:
;; search(unsigned start, byte length)
        pop     bc
        pop     de ;; start
        ld      (start),de
        pop     de ;; searchlen
        push    de
        push    de
        push    bc

        ld      a,e
        or      a
        jp      z,FoundAtStart
        ld      (searchlen),a

        ld      b,a
        ld      hl,searchstring

CopySearchTop:
;; copy search string, raising case
        ld      a,(hl)
        cp      'z'+1
        jr      nc,NotLC
        cp      'a'
        jr      c,NotLC
        and     0ffh-20h
NotLC:
        ld      (hl),a
        inc     hl
        djnz    CopySearchTop

        call    $aux_pagein

        ld      hl,(_file_length)
        ld      b,0
        ld      a,(searchlen)
        ld      c,a
        or      a
        sbc     hl,bc
        jp      c,NotFound
        ld      bc,(start)
        sbc     hl,bc
        jr      c,NotFound      ;; filelength<=start+searchlen-1 ?
        inc     hl

;;        ld      hl,(_file_length)
;;        ld      bc,(start)
;;        or      a
;;        sbc     hl,bc

;; hl=number of locations to check

        ld      c,l
        ld      b,h

        ld      hl,(start)
        ld      de,08000h
        add     hl,de

        ld      a,(searchstring+1)
        ld      (char2),a

SearchTop:
        ld      d,07fh-20h  ;; compare modulo case and wrap
SearchTop1:
        ld      a,(searchstring)
        and     d
        ld      e,a

;;        cpir

InnerSearch1Top:
        ld      a,c
        or      b
        jr      z,NotFound
        ld      a,(hl)
        xor     e
        and     d
        inc     hl
        dec     bc
        jp      nz,InnerSearch1Top

MatchedFirst:
        ld      a,0
searchlen equ $-1
        dec     a
        ld      e,a
        jr      z,Found1

        ld      a,0
char2 equ $-1
        xor     (hl)
        and     d

        jp      nz,SearchTop1

        dec     e
        jr      z,Found1

        push    hl
        push    bc

        inc     hl
        ld      b,e
        ld      de,searchstring+2

InnerSearchLoop0:
        ld      a,(de)
        xor     (hl)
        and     07fh-20h
        jp      nz,NotMatch
        inc     hl
        inc     de

        djnz    InnerSearchLoop0

        pop     bc
        pop     hl

        jp      Found1

NotMatch:
        pop     bc
        pop     hl

        jp      SearchTop

Found1:

        push    hl
        push    bc
        dec     hl

        ld      a,(searchlen)
        ld      b,a

;; check more precisely, doing case comparisons more exactly

        ex      de,hl

        ld      hl,searchstring

exact_compare:
        ld      a,(de)
        and     7fh
        cp      'z'+1
        jr      nc,NotLC2
        cp      'a'
        jr      c,NotLC2
        and     0ffh-20h
NotLC2:
        cp      (hl)
        jr      nz,NotExactMatch
        inc     hl
        inc     de
        djnz    exact_compare

        pop     bc
        pop     hl
        dec     hl

        ld      bc,8000h
        or      a
        sbc     hl,bc
        jp      $aux_pageout

NotExactMatch:
        pop     bc
        pop     hl
        jp      SearchTop

NotFound:
        ld      hl,-1

        jp      $aux_pageout
FoundAtStart:
        ld      hl,(start)
        jp      $aux_pageout

check_err:
        defs 1

init1:
        call    $aux_pagein
        ld      bc,(_file_length)
        ld      hl,8000h
        exx
        ld      de,range181_255
        ld      b,0
        exx
        ret

_import_file:
        xor     a
        ld      (check_err),a
        call    init1
checkLoop:
        ld      a,c
        or      b
        jr      z,checkDone
        ld      a,(hl)
        bit     7,a
        jr      nz,check_hi
        cp      32
        jp      c,seeIf13
checkOK:
        inc     hl
        dec     bc
        jp      checkLoop
checkDone:
        ld      a,(check_err)
        ld      l,a
        jp      $aux_pageout
seeIf13:
        cp      13
        jp      z,checkOK
isError:
        ld      a,'*'
        ld      (check_err),a
putIn:
        ld      (hl),a
        jr      checkOK
check_hi:
        cp      128
        jr      nz,Not128
        ld      a,12
        jr      putIn
Not128:
        cp      163
        jr      nz,Not163
        ld      a,30
        jr      putIn
Not163:
        sub     181
        jr      c,isError
        exx
        ld      l,e
        ld      h,d
        ld      c,a
        add     hl,bc
        ld      a,(hl)
        exx
        cp      255
        jr      nz,putIn
        jr      isError

_export_file:
        call    init1
        exx
        ld      de,_xlat_table
        exx
exportLoop:
        ld      a,c
        or      b
        jp      z,$aux_pageout
        ld      a,(hl)
        and     7fh
        cp      32
        jr      c,fixUp
exportSave:
        ld      (hl),a
        inc     hl
        dec     bc
        jr      exportLoop
fixUp:
        exx
        ld      l,e
        ld      h,d
        ld      c,a
        add     hl,bc
        ld      a,(hl)
        exx
        jr      exportSave

_clear_wrap_to_end:
;; clear_wrap_to_end(unsigned offset);
        call    $aux_pagein
        pop     hl
        pop     de    ;; offset
        push    de
        push    hl

        ld      hl,(_wrapped_length)
        or      a
        sbc     hl,de

        jp      c,$aux_pageout  ;; nothing wrapped beyond this point

        ld      c,l
        ld      b,h   ;; length of wrapping area

        ex      de,hl ;; hl=offset
        ld      (_wrapped_length),hl

        ld      de,08000h
        add     hl,de     ;; memory offset
ClearWrapLoop:
        ld      a,c
        or      b
        jp      z,$aux_pageout
        ld      a,(hl)
        and     07fh
        ld      (hl),a
        inc     hl
        dec     bc
        jp      ClearWrapLoop

_wrap_line:
;; unsigned wrap_line(unsigned offset);
;;    wrap the line at the offset
;;    return offset of start of next line
        call    $aux_pagein
        pop     bc        ;; return address
        pop     de        ;; offset
        push    de
        push    bc

        ld      hl,(_file_length)
        or      a
        sbc     hl,de

        ld      c,l
        ld      b,h

        ex      de,hl    ;; hl=offset

        ld      de,08000h
        add     hl,de

        xor     a        ;; line position = 0
        ld      d,a      ;; last space character (de=0)
wrap_loop:
        push    de
        ld      d,a
        ld      a,c
        or      b
        jp      z,dechl_popde_wrap_end_here    ;; last char of file
        ld      a,d
        push    hl
        ld      e,(hl)
        ld      d,0
        ld      hl,_width_table
        add     hl,de
        add     a,(hl)
        pop     hl
        cp      SCREEN_WIDTH+1
        jp      nc,wrap_end
        ld      d,a        ;; current x-coordinate
        ld      a,e        ;; current character
        cp      13
        jp      z,popde_wrap_end_here
        cp      ' '
        ld      a,d        ;; current x-coordinate
        pop     de
        inc     hl
        dec     bc
        jp      nz,wrap_loop ;; if not space, continue loop
is_space:
        ld      e,l
        ld      d,h        ;; save location of space
        dec     de
        jp      wrap_loop

PreviousCharSet:
        dec     hl
        jp      set_eol
wrap_end:
        pop     de
        ld      a,d
        or      e
        jr      z,PreviousCharSet
        ex      de,hl   ;; hl = location of last space
        jp      set_eol
dechl_popde_wrap_end_here:
        dec     hl
popde_wrap_end_here:
        pop     de
set_eol:
        ld      a,(hl)
        or      80h
        ld      (hl),a
        ld      bc,8000h-1
        sbc     hl,bc   ;; carry is already clear
        jp      $aux_pageout

global _xlat_override
;; unsigned grab_line(unsigned offset);
;;    grab a line, assuming it was paginated, returning offset of the next one
_grab_line:
        ld      a,0
_xlat_override equ $-1
        or      a
        jr      z,doxlat
        xor     a
        ld      (_do_grab_transl),a
doxlat:
        call    $aux_pagein
        pop     de
        pop     hl
        push    hl
        push    de
        ld      bc,8000h
        add     hl,bc
        ld      de,_line_buf
        exx
        ld      b,0
        ld      de,_xlat_table
        exx
LineTop:
        ld      a,(hl)
        ld      c,a
        and     7fh
        cp      32
_do_grab_transl equ $-1
        jp      c,xlat
        or      a
        jr      z,xlat0
OK:
        ld      (de),a
        rl      c
        jr      c,EOL
        inc     hl
        inc     de
        jp      LineTop
xlat0:
;        ld      a,(_do_grab_transl)
;        or      a
;        jr      z,OK ;; do not translate, a=0
        ld      a,127
        jr      OK
xlat:
        exx
        ld      l,e
        ld      h,d
        ld      c,a
        add     hl,bc
        ld      a,(hl)
        exx
        jp      OK
EOL:
        inc     de
        xor     a
        ld      (de),a
        ld      bc,8000h-1
        sbc     hl,bc
        jp      $aux_pageout

_insert:
;; void insert(unsigned offset,unsigned length)
        pop     hl
        pop     de  ;; offset
        pop     bc  ;; length (to insert)
        push    bc
        push    de
        push    hl

        ld      a,c
        or      b
        ret     z


        ld      hl,(_file_length)
        or      a
        sbc     hl,de

        ret     z
        ret     c

        call    $aux_pagein

        push    hl     ;; amount of text to move

        ld      hl,(_file_length)
        ld      de,8000h-1
        add     hl,de
        push    hl
        add     hl,bc
        ex      de,hl  ;; destination

        pop     hl     ;; source
        pop     bc     ;; length

        lddr

        jp      $aux_pageout

_delete:
;; void delete(unsigned offset,unsigned length)
        pop     hl
        pop     de  ;; offset
        pop     bc  ;; length (to insert)

;        ld      de,60
;        ld      bc,1

        push    bc
        push    de
        push    hl

        ld      a,c
        or      b
        ret     z

        ld      hl,(_file_length)

        ex      de,hl
        add     hl,bc
        ex      de,hl  ;; de=offset+length

        sbc     hl,de  ;; file_length-(offset+length)

        ret     c
        ret     z

        call    $aux_pagein

        push    hl     ;; amount of text to move

        ld      hl,8000h
        add     hl,de  ;; hl=8000+offset+length
        ld      e,l
        ld      d,h    ;; de=8000+offset+length
        sbc     hl,bc  ;; hl=8000+offset
        ex      de,hl  ;; de=8000+offset,hl=8000+offset+length

        pop     bc     ;; length

        ldir

        jp      $aux_pageout

_find_next_line:
;; find_next_line(unsigned offset);
;;   Find the next end of line.  This assumes the text has been
;;   paginated past the present offset, else things may crash.
;;   Not to be called at end of file.
        pop     de
        pop     hl  ;; offset
        push    hl
        push    de

        call    $aux_pagein

        ld      de,8000h
        add     hl,de

EOL_Loop:
        ld      a,(hl)
        inc     hl
        and     80h
        jp      z,EOL_Loop

        ld      de,8000h
        sbc     hl,de

        jp      $aux_pageout

_find_prev_line:
;; find_prev_line(unsigned offset);
;;   Find the previous end of line.
        pop     de
        pop     hl  ;; offset
        push    hl
        push    de

        call    $aux_pagein


        ld      c,l
        ld      b,h

        ld      a,c
        or      b
        jp      z,$aux_pageout

        ld      de,8000h-1
        add     hl,de

        dec     bc

        ld      a,c
        or      b
        jr      z,done

        ld      a,(hl)
        cp      13.or.80h
        dec     hl
        jr      nz,PrevLoop

        dec     bc
        ld      a,(hl)
        and     80h
        jr      nz,inc_done
        ld      a,c
        or      b
        jr      z,done

        dec     hl

PrevLoop:
        dec     bc
        ld      a,c
        or      b
        jr      z,done
        ld      a,(hl)
        and     80h
        jr      nz,inc_done
        dec     hl
        jp      PrevLoop
inc_done:
        inc     hl
done:
        ld      de,8000h
        sbc     hl,de

        jp      $aux_pageout

