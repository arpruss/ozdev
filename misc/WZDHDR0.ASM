;   WZDHDR0.ASM
;
;   Header file to be included with Assembler routines designed for the Wizard
;   and assembled using TASM31
;
;   Puts all the header information on the front end of an object file
;   so the object file can be downloaded directly to the wizard.
;
;   See the program SERIAL01.ASM for an example of how to use this file
;
;   Included in the header is the one line of BASIC
;   used to call the assembler routine
;
;      100    CALL   &Hxxxx
;
;      The value xxxx is calculated and put in the basic statement.
;
;      Additional BASIC statments could be added later which would
;      make this start location change.
;
;      NOTE: THIS IS NOT A BASIC COMPILER.  THE BASIC CODE IS ENTERED
;            MANUALLY WITH THE .DB ASSEMBLER DIRECTIVE
;
;    Use this program for your own education and enjoyment
;    There are no warrantees, expressed or implied
;    so use at your own risk.
;    This program is unsupported freeware.
;       by Rob Jahnke
;
	.org 	0000h
HDR:    .DB     "<SHARP WZD DATA>\r\n"
        .DB     "<DATA TYPE>\r\n","MY PROGRAMS\r\n"
	.DB     "</DATA TYPE>",13,10
        .DB     "<TITLE>\r\n"
	TITLE
        .DB     "\r\n</TITLE>\r\n"
        .DB     "<CATEGORY>",13,10,"PROGRAM",13,10
        .DB     "</CATEGORY>",13,10
        .DB     "<DESCRIPTION>",13,10
        DESCRIP 
        .DB    "\r\n</DESCRIPTION>",13,10
        .DB     "<CONTENT>",13,10,"BIN_PROG_1",13,10
        .DB    "</CONTENT>",13,10
        .DB    "<DATA>",13,10,"PFILE:"
	NAME	
	.db    ".BAS",13,10
        .DB     "</DATA>",13,10
; 			Program name must be "PFILE:Bxxxxxxx.BAS"          
        .DB     "<BIN>",13,10
;	     	    ICON BINARY DATA (12 ROWS - 2 BYTES/ROW) 
;                           (BYTES "FLIPPED" - FIRST 3 NIBBLES USED)
;                           (FIRST BYTE IS LENGTH IF ICON DATA)
        .DB     18H,60H,00H,20H,01H,20H,01H,0F0H,01H
        .DB     38H,00H,1CH,02H,0EH,05H,87H,08H,85H,08H
        .DB     8DH,0FH,81H,08H,8FH,08H
;
;           BASIC PROGRAM
;           100 CALL &Hxxxx
;           
;           This is the tokenized basic program. Generated with BACON
;
;PGM MUST START HERE WITH .ORG    0A000H  ;Start of user pgm
;
	.ORG    0A000H
MYPGM:  .DB     00H,64H,09H,0FEH      ;100 CALL &Hxxxx
        .DB     31H,26H,48H
;Code to Put ASCII value of S0000 here	
	.DB	((((S0000-0A000h)/4096)&00Fh)+41h)
	.DB	((((S0000-0A000h)/256)&00Fh)+30h)+(((((S0000-0A000H)/256)&00Fh)>9)*7h)
	.DB	((((S0000-0A000h)/16)&00Fh)+30h)+(((((S0000-0A000H)/16)&00Fh)>9)*7h)	
	.DB	((((S0000-0A000h)&00Fh))+30h)+((((S0000-0A000H)&00Fh)>9)*7h)
        .DB     0DH
        .DB     75H,31H,03H,0FEH      ;END OF BASIC TAG
        .DB     5AH,0FH          
S0000	.equ	*
	.codes
	.list
