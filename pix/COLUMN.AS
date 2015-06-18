psect lowpage

MAXLENGTH equ 2048

global _one_column,_show_column,__ozactivepage

_one_column:
;; defs MAXLENGTH
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc
*include 256zeros.inc

length:         defb 0
start:          defw 0
slug:           defb 0
current_pos:    defw 0

_show_column:
;; show_column(byte x,byte slug,unsigned start,unsigned length)
        pop     hl
        pop     bc                    ; x
        ld      b,0a0h
        ld      (current_pos),bc
        pop     bc                    ; slug
        ld      a,c
        ld      (slug),a
        pop     bc                    ; start
        ld      (start),bc
        pop     bc
        ld      a,c
        ld      (length),a            ; length
        push    bc
        push    bc
        push    bc
        push    bc
        push    hl

        in      a,(3)
        ld      c,a
        in      a,(4)
        ld      b,a
        push    bc

        ld      bc,(__ozactivepage)
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a

        ld      hl,_one_column
        ld      bc,(start)
        exx
        ld      hl,(current_pos)
        ld      de,30
        exx
;; skip past starting matter
        ld      a,(slug)
        ld      d,a
StartSkip:
        ld      a,c
        or      b
        jr      z,Started
        ld      a,(hl)
        cp      d
        jr      z,StartSlug
StartContinue:
        inc     hl
        dec     bc
        jp      StartSkip
StartContinue1:
        ld      a,(slug)
        ld      d,a
        inc     hl
        dec     bc
        jp      StartSkip
Started:
        ld      a,(length)
        ld      b,a
        jp      MainLoop
StartSlug:
        inc     hl
        ld      e,(hl)  ;; value
        inc     hl
        ld      d,(hl)  ;; count
StartSlugLoop:
        ld      a,d
        or      a
        jr      z,StartContinue1
StartSlugLoop0:
        dec     bc
        ld      a,c
        or      b
        jr      z,PutUp
        dec     d
        jp      nz,StartSlugLoop0
        jp      StartContinue1
PutUp:
        ld      a,(length)
        or      a
        jr      z,GetOut
        ld      b,a
        ld      a,e
PutUpLoop:
        exx
        ld      (hl),a  ;; erased:still bad
        add     hl,de
        exx
        dec     b
        jr      z,GetOut
        dec     d
        jr      nz,PutUpLoop
        inc     hl
MainLoop:
        ld      a,(slug)
        ld      c,a
        ld      a,b
        or      a
MainLoop0:
        jr      z,GetOut
MainLoop1:
        ld      a,(hl)
        inc     hl
        cp      c
        jr      z,DoSlug
        exx
        ld      (hl),a         ;; erased:still bad
        add     hl,de
        exx
        djnz    MainLoop1
GetOut:
        pop     bc
        ld      a,c
        out     (3),a
        ld      a,b
        out     (4),a
        ret
DoSlug:
        ld      e,(hl)  ;; value
        inc     hl
        ld      d,(hl)  ;; count
        inc     hl
        ld      a,e
        exx
        ld      (hl),a       ;; trying now! -- halts without
        add     hl,de
        exx
SlugLoop:
        dec     b
        jr      z,GetOut
        ld      a,d
        or      a
        jr      z,MainLoop1
        ld      a,e
SlugLoop3:
        exx
        ld      (hl),a       ;; trying now!!
        add     hl,de
        exx
        dec     b
        jr      z,GetOut
        dec     d
        jp      nz,SlugLoop3
        jp      MainLoop1


