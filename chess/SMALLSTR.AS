;;
;; Implement _strlen,_strcpy,_strcmp by calling OS
;;

psect text
global _strlen,_strcpy ;;,_strcmp
_strlen:
        pop     de
        pop     hl
        push    hl
        push    de
        jp      7f0ch

_strcpy:
        pop     bc
        pop     hl
        pop     de
        push    de
        push    hl
        push    bc
        jp      7f09h

;_strcmp:
;        pop     bc
;        pop     de
;        pop     hl
;        push    hl
;        push    de
;        push    bc
;        jp      7f15h
