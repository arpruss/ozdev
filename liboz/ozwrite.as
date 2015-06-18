global _ozwritefile,$swapupperpages,Model32k,$copytopage,$sp$,__ozmodel
global _ozdetectmodel,$filetypetable,_ozinitfs,__ozfsdirty
global __ozlastrecnum,_ozgetlastrecnum
global __ozindexbycounts,set_a000_400,restore_a000
global $clearbacklighttimer

Limit EQU 2048
osEnableKeyboard equ 0c00fh

psect midpage
len:
        defw 0
data_address:
        defw 0
type:
        defb 0
saved_7:
        defb 0
retval:
        defb 0
orig_sp:
        defw 0

psect lowpage
global _ozwritefile
;; ozwritefile(char *file, unsigned len, byte type)
_ozwritefile:
        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel
        push    ix
        ld      ix,2
        add     ix,sp

        ld      l,(ix+4)   ; len
        ld      h,(ix+5)

        ld      (len),hl

        ld      de,-Limit  ; size limit
        add     hl,de
        jp      c,file_too_big

        ld      bc,(len)

        ld      a,(ix+6)
        ld      (type),a

        ld      a,Model32k
        or      a
        jp      z,ozwritefile16k

        ld      hl,($sp$)
        ld      (orig_sp),hl
        or      a
        sbc     hl,bc
        ld      ($sp$),hl
        ld      bc,0a000h-0e000h ;; adjust for a-space
        add     hl,bc
        ex      de,hl
        ld      hl,409h  ;; page to copy to
        push    hl
        ld      l,(ix+2)
        ld      h,(ix+3) ;; file
        ;; copy bc bytes at hl to 409:de
        ld      bc,(len)
        call    $copytopage ;; copy the file into temporary storage
        pop     hl

        di
        call    $swapupperpages
        call    $clearbacklighttimer

        in      a,(7)
        ld      (saved_7),a
        ld      a,0ffh  ;; disable all interrupts
        out     (7),a

        ld      hl,0
        add     hl,sp

        ld      (data_address),hl

        im      1

        call    AddFile

        di
        ld      a,l
        ld      (retval),a
        ld      a,(osEnableKeyboard)
        and     0ffh-7
        ld      (osEnableKeyboard),a
        call    $swapupperpages
        ld      a,(saved_7)
        out     (7),a
        im      2
        ei

        ld      hl,(orig_sp)
        ld      ($sp$),hl
        ld      a,(retval)
        ld      l,a

DoneAll:
        pop     ix
        ld      h,0
        push    hl
        call    _ozinitfs
        pop     hl
        ld      a,1
        ld      (__ozfsdirty),a
        ret

psect text
ozwritefile16k:
        ld      hl,0
        add     hl,sp
        sbc     hl,bc
        ld      sp,hl
        ld      (data_address),hl

        ex      de,hl

        ld      l,(ix+2)   ; file
        ld      h,(ix+3)

        ldir

        call    AddFile
        ei

        dec     ix
        dec     ix
        ld      sp,ix

        jp      DoneAll

file_too_big:
        pop     ix
        ld      hl,0ffh
        ret

psect lowpage
AddFile:
push    ix
ld      ix,0
add     ix,sp
ld      hl,-12
add     hl,sp
ld      sp,hl

;; at ix-12 and ix-11 = sp and sp+1 : RecNum
;; at ix-10 and ix-9  = sp+2 and sp+3 : IDNum
;; at ix-8 = sp+4 : index info block copy

;; zero RecNum and IDNum
xor     a
ld      (hl),a
inc     hl
ld      (hl),a
inc     hl
ld      (hl),a
inc     hl
ld      (hl),a
inc     hl

ex      de,hl                  ;; copy of index info block
ld      hl,type
ld      l,(hl)
ld      h,a       ;; recall that a=0
add     hl,hl     ;; hl=2*type
add     hl,hl     ;; hl=4*type
add     hl,hl     ;; hl=8*type
ld      bc,($filetypetable)    ;; index info block
add     hl,bc
ld      bc,8
ldir

ld      hl,(data_address)
push    hl
ld      hl,(len)
push    hl                     
ld      hl,4
add     hl,sp
push    hl                     ;; RecNum pointer
ld      h,a                    ;; recall that a=0 still
ld      a,(type)
ld      l,a                    ;; hl=type
ld      bc,__ozindexbycounts
add     hl,bc
ld      c,(hl)                 ;; c=number of indexes for type
ld      hl,6+4
add     hl,sp                  
ex      de,hl                  ;; de=pointer to copy of index info block
call    7f73h                  ;; write file
pop     de
pop     de
pop     de

ld      l,a
or      a
jr      nz,Exit

ld      a,(type)
ld      e,a

call    set_a000_400

ld      d,a          ;; a=0 still
                     ;; de=type
ld      hl,0bf25h    ;; list of current index numbers (1-3) for filetypes
add     hl,de
ld      l,(hl)
ld      h,a          ;; (a=0 still) hl=current index number for the type

call    restore_a000

add     hl,hl      ;; hl=2 * current index number
ld      bc,($filetypetable)
add     hl,bc
ld      e,(hl)
inc     hl
ld      d,(hl)     ;; de=RecNum of current index

ld      hl,0
add     hl,sp      ;; hl=pointer to RecNum
ld      c,(hl)
inc     hl
ld      b,(hl)     ;; bc=RecNum
ld      (__ozlastrecnum),bc
inc     hl         ;; hl=pointer to IDNum
push    hl
call    07f7ch
pop     bc

ld      l,0

Exit:
ld      sp,ix
pop     ix
ret

          ;;
          ;; This specifies the number of indexes (sorting methods)
          ;; for each Wizard file type.  Thus, _TEL has three: firstname,
          ;; lastname, company (not necessarily in this order, and the
          ;; ordering requires the indexby program).
          ;;
psect lowpage
_ozgetlastrecnum:
        ld      hl,0
__ozlastrecnum equ $-2
        ret
