global _ozunlink,$swapupperpages,$32kinton,$32kintoff,__ozmodel,_ozdetectmodel
global $filetypetable,_ozinitfs
global __ozindexbycounts
global $clearbacklighttimer

psect midpage
type: defb 0
retval: defb 0
recnum: defw 0

;; ozunlink(byte type,unsigned recnum);
_ozunlink:
        ld      a,(__ozmodel)
        xor     0ffh
        call    z,_ozdetectmodel
        push    ix
        ld      ix,0
        add     ix,sp
        ld      a,(ix+4)
        ld      (type),a
        ld      a,(ix+6)
        ld      (recnum),a
        ld      a,(ix+7)
        ld      (recnum+1),a
        call    $32kintoff
        call    $swapupperpages
        call    $clearbacklighttimer

;ld      a,8
;ld      hl,8c04h ;; pre-770: 8D67h
;call    4ah

       ld      ix,0
       add     ix,sp
       ld      hl,-8
       add     hl,sp
       ld      sp,hl
       ld      bc,(recnum)
       push    bc
       ex      de,hl     ;; de=pointer to 8 byte area
       push    de
       ld      a,(type)
       ld      l,a
       ld      h,0
       add     hl,hl
       add     hl,hl
       add     hl,hl
       ld      bc,($filetypetable)
       add     hl,bc     ;; hl=c088+8*type
       ld      bc,8
       ldir
       pop     de        ;; de=pointer to temporary area
       ld      hl,__ozindexbycounts
;;;       ld      a,(type)  ;; (a=type still)
       ld      c,a
       ld      b,0
       add     hl,bc
       ld      c,(hl)
       di
       call    7f76h     ;; delete file
       ld      sp,ix
       ld      (retval),a
       di
       call    $swapupperpages
       call    $32kinton


       ei

       pop     ix
       ld      a,(retval)
       ld      l,a
       ld      h,0
       push    hl
       call    _ozinitfs
global  __ozfsdirty
       ld      a,1
       ld      (__ozfsdirty),a
       pop     hl
       ret

