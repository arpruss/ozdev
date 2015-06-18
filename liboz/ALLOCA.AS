psect text

global ___y_alloca_19828a_

;;
;; void *alloca(unsigned size);  /* needs alloca.h */
;;

___y_alloca_19828a_:
        pop     bc ;; ret address
        pop     de ;; size
        ld      hl,-2 ;; adjust for the size parameter
        add     hl,sp ;;
        or      a
        sbc     hl,de ;; allocate size+2 bytes on stack
        ld      sp,hl
        inc     hl
        inc     hl ;; point after de
        push    bc ;; ret address
        ret
