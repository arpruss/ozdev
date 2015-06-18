psect text
LINE_BUF_LEN equ 124
MAX_RECNUMS equ 10
MAX_LINES_PER_SCREEN equ 20
TITLE_LENGTH equ 20
DATE_LENGTH equ 9
MAX_INDEX_COUNT equ 7fh
global _saved_line,_recnums,_changed,_title,_date,_typedin
global _index_lengths
_saved_line:
_recnums equ _saved_line+LINE_BUF_LEN
_changed equ _recnums+2*MAX_RECNUMS
_title equ _changed+MAX_LINES_PER_SCREEN
_date equ _title+TITLE_LENGTH+1
_typedin equ _date+DATE_LENGTH
_index_lengths equ _typedin+TITLE_LENGTH+1
*include init.aso
psect text
len_actual equ $-_saved_line
len_needed equ _index_lengths+MAX_INDEX_COUNT-_saved_line
cond len_needed.ult.len_actual
BUG! Too little stuff in init.aso
endc

