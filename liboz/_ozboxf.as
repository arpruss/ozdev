;
; By Lawrence Chitty
;

    psect lowpage
    global __ozfilledbox , __ozactivepage
     
     
     
;_ozfilledbox(byte x , byte y , byte width , byte height , byte color)

__ozfilledbox:

;;;save ix and set to sp.
    push   ix
    ld     ix , 0
    add    ix , sp

;;;switch in video page    
    
    ld     a,(__ozactivepage)
    out    (3),a

;;;do the 'code modifications'  and put the color mask modifier in e - this isn't touched again.  
    
    ld     e,000h                      ;color mask modifier initial set to 00
    ld     b , (ix + 12)
    djnz   notblack
    ld     a , 0b1h                    ;or c command
    jr     modfinish

notblack:
    djnz   white
    ld     a , 0a9h                    ;xor c command
    jr     modfinish

white:                      
    ld     a , 0a1h                     ;and c command
    dec    e                            ;color mask mask modifier = 0ffh

modfinish:
    ld     (codemod) , a              ;modify the columnloop

    ld     a,(ix+10)                 ; height
    ld     ($height),a


;;;get the starting byte and set up alternate registers

    exx
    ld     d , 0
    ld     e , (ix + 6) ; y co-ord
    ld     h , d
    ld     l , e
    add    hl , hl      ; *2
    add    hl , hl      ; *4
    add    hl , hl      ; *8
    add    hl , hl      ; *16
    sbc    hl , de      ; *15
    add    hl , hl      ; *30
    ld     d , 0a0h
    ld     a , (ix + 4) ; x co-ord
    ld    e,a
    srl   e
    srl   e
    srl   e
    add    hl , de
    ld     de,30
    exx
    



;;;Do the columns - initialize

    ld     d , (ix + 8)                ;store the width in d
    ld     b , 0                      ;bc pair is used to index masks, so start to set up

;;;do left side
    and    007h                        ;a still contains x coord - gives number of bits on left side.
    ld     c , a                      ;store in c before continueing  (left column offset)
    jr     z , middlesection          ;if no bits, abort left side
    ld     hl , masks - 1
    add    hl , bc
    add    a , d
    sub    8                          ;see if all bits contained in mid-byte
    ld     a , (hl)
    jr     c , singlebytewidth        ;jump all bits are contained in mid-byte
    call   drawcolumn


;;;do middle section            
;;;first, get bits in right column

middlesection:
    ld     a , d
    add    a , c
    ld     d , a                      ;store result in e for use by right column

    ld      h,0f8h
    and   h
    sub   c
    and   h			      ; ensure lowest three bits are 0
    rra				      ; so the rra just shifts a right logically
    rra
    rra                               ; divide by 8

    jr     z , rightcolumn    
    ld     h, 0ffh                       ; note that b=0 here
    ld     b , a                      ;store width for loop
middleloop:
    ld     a , h                      ;load bitmask for drawcolumn (0)
    call   drawcolumn
    djnz   middleloop
;; b=0 at end of djnz ;;


;;;do right column

rightcolumn:
    ld     a , d                      ;d is now the width + left column offset.
    and    007h
    jr     z , tidyup
    ld     c , a
    ld     a , 0ffh
    jr     finishright

singlebytewidth:

    ld     h , a                      ;invert and store left mask
    ld     a , c                      ;load left column offset    
    add    a , d                      ;add width
    ld     c , a
    ld     a , h                      ;restore left mask

finishright:
    ld     hl , masks - 1
    add    hl , bc 
    xor    (hl)
    call   drawcolumn
 


tidyup:
    ld      a,7
    out     (3),a
    pop     ix
    ret



;;;Column drawing routine
;;;call with mask in a. 

drawcolumn:
    xor    e                          ;xor with color modifier
    exx
    push   hl
    ld     c , a                      ;load mask into c
    ld     b , 0
$height equ $-1    ;load height (self-modifying)
drawcolumnloop:
    ld     a , (hl)    
codemod:
    or     c                          ;this line is modified between or / and / xor
    ld     (hl) , a
    add    hl , de                    ;using alternate register set so de already set to 30
    djnz   drawcolumnloop
    pop    hl
    inc    hl
    exx 
    ret


masks:

    defb  254 , 252 , 248 , 240 , 224 , 192 , 128


