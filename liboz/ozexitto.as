global _ozexitto
global _ozkeyclear
global _ozungetch
global _exit
psect text
_ozexitto:
call _ozkeyclear
pop bc ;; retval
pop hl ;; key

push hl ;; put two copies of key on stack
push hl
call _ozungetch
pop hl  ;; use previous copy of key on stack as argument
call _ozungetch
jp _exit

psect data
psect bss
