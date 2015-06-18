global _main
psect text
_main:
defm 'Hello'

psect data
org 08125h
defm 'Hi!'
