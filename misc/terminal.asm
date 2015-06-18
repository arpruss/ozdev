;
;  Terminal Emulator for ZQ700
;       25/06/2000
;       Mike Field
;
;
;  Assemble with TASM31 as follows
;       C:>TASM -80 -g3 <filename.asm>  <filename.wzd>  
;               This generates a text list file: <filename.lst>
;		       and a binary object file: <filename.wzd>                          
;
;               This wzd file is ready to download to the wizard.
;
;       Note:  You will need the file wzdhdr0.asm in the same directory           
;
;**************************************************************************
; This program is unsupported freeware
;
;    Based on wzdhdr0.asm by Rob Jahnke

	.nocodes
#define TITLE   .Text   "Terminal      "                  ;14 Chrs max
#define	NAME	.Text	"Bxxx0001"			; B + 7 Chrs
#define DESCRIP .Text   "Terminal Emulator - Connect up to a Unix box!"; use \r\n for cr lf
	.nolist
#include wzdhdr0.asm

SERSTAT     .EQU    45H
SIN         .EQU    40H
SOUT        .EQU    40H
KEYLINESEL  .EQU    17
KEYLINEDATA .EQU    16

FIRSTCHAR   .EQU    32  ; ASCII codes that are defined in the font table
LASTCHAR    .EQU    126

BYTESPERLINE .EQU   30  ; Display arributes
LINESPERROW  .EQU    8
LINEONSCREEN .EQU   10
CHARSPERLINE .EQU   30  ; May be different if chars are not 8 wide
;
;
;=================== The entry setup ===============
START       PUSH    AF          ;Save all the registers
            PUSH    BC
            PUSH    DE
            PUSH    HL

            DI                  ;Disable interupts

            IN      A,(01H)     ;Save the Memory Management Regs in BC
            LD      C,A         ; This stuff is only here to demonstrate
            IN      A,(02H)     ; how to do it. It's not needed in this pgm
            LD      B,A

            PUSH     BC         ; Put the saved regs on the stack

            LD      A,0FCH      ; Set the page registers so that
            OUT     (01H),A     ; 8000H is mapped to the first
            LD      A,03H       ; location in graphics memory
            OUT     (02H),A 
;=================== Main program ===============
            ; Clear the screen
            LD      A,13
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            CALL    DISPLAY
            SUB     A
            LD      (CURSORY),A

            ; Send a welcome string!
            LD      HL,WELCOMEMSG-1
NEXTWCHAR   INC     HL
            LD      A,(HL)
            CALL    DISPLAY
            OR      A            ;Check for 00 Terminator
            JR      NZ, NEXTWCHAR
            
            ; Set up for the main loop
            LD      HL,MULTICHARS ; Point multichar strings to empty str
            JR      MAINLOOP      ; Jump into the middle of it

;======= the top of the main loop ==========
; During the main loop HL must be keep safe - it holds the
; "Multi character string" pointer.

; Read the character and update the display
SERCHAR     IN      A,(SIN)     ;Get the Serial Character into A

; As well as updating the display, place the charater to be echoed 
; back to the host
QUEUECHAR   LD      IX,CHARTOSEND ; Store it to be send so that it is 
            LD      (IX),A        ; echoed
            CALL    DISPLAY       ; Place it on the screen

;
; See if there is a character to read
;
MAINLOOP    IN      A,(SERSTAT) ; Check Serial Status
            AND     1
            JR      NZ,SERCHAR  ; Read the byte and process it.

            ; See if there is still part of a
            ; mulitcharacter string to send
            ; This is used for arrow keys and escapes.
            LD      A,(HL)
            OR      A            ;Check for 00 Terminator
            JR      NZ, SENDSTRCHAR     ; Nope - don't send anything
;
; See if there is a char to send
;
NEXT1       LD      A,(CHARTOSEND)
            OR      A           ; Check for 00 - nosend 
            JR      NZ,CLEARANDSEND
;
; See if a key is pressed down
;
            LD      A,(KEYSEL)
            OR      A
            JR      NZ,TESTKEYUP
;
; See if a new key has been pressed
;
            LD      A,1             ; Select first row of keys
            LD      B,A             ; B - Row select - hard code to 1
            LD      C,A             ; C - Key Mask   - hard code to 1
            OUT     (KEYLINESEL),A  ; Set the line select
            IN      A,(KEYLINEDATA) ; Get the First row of data
            AND     C               ; Check for ESC key
            JR      Z, MAINLOOP     ; No key down - back to top
            LD      (KEYSELMASK),BC ; Save the mask and select of the key
            JR      MAINLOOP        ; Back to the top

;===== Out of the main line of the program =======
; See if the key has been lifted
TESTKEYUP   LD      BC,(KEYSELMASK)
            LD      A,B
            OUT     (KEYLINESEL),A   ;Select first row of keys
            IN      A,(KEYLINEDATA)  ;Get the First row of data
            AND     C                ;Check for the matching Mask
            JR      NZ, MAINLOOP     ;Still set  - key still down
            LD      B,C              ;reset the key pressed flag (C is 0!)
            LD      (KEYSELMASK),BC
            JR      EXIT             ; Really this should jump back to the
                                     ; top - but I'll make it exit

            ; Put this anywhere really
SENDSTRCHAR INC     HL
            JR      SENDACHAR ;If not then jump output the character

CLEARANDSEND  ; Send out character in chartosend - drops through to SENDACHAR
            LD      A, (CHARTOSEND)
            LD      IX, CHARTOSEND  ; Clear the char to send byte
            LD      (IX), 0

            ; Send a character in 'A' to the serial port,
            ; Part of the main loop!!!!
            ;   INPUT  A = character to send
            ;   USES   B, IX
SENDACHAR   LD      D,A               ; Save the char to send
WAIT        IN      A,(SERSTAT)       ; Check CLEAR TO SEND
            AND     20H 
            JR      Z,WAIT            ; Wait until clear to send

            LD      A,D              ; Send it to the Serial Out Port
            OUT     (SOUT),A
            JR      MAINLOOP     ; Go back for next byte
;=================== End of the mail loop propper            
;=================== Exit the program ===============
EXIT        POP     BC          ;Get the saved MMU regs
            LD      A,C         ;Restore the MMU regs
            OUT     (01H),A
            LD      A,B
            OUT     (02H),A 
        
            EI                  ;Enable Interupts
        
            POP     HL          ;Restore the regs
            POP     DE
            POP     BC
            POP     AF
            RET

;===========================================================================
; DISPLAY - Subrotine to put a character on the screen and update the
;           cursor positions
;  A = character to show
;  All others are safe
;===========================================================================

DISPLAY     PUSH    HL              ; Save registers
            ; HL  used to calc the index the Line start tables
            ;             calc the index in the font tables
            PUSH    BC
            ; BC  used to hold the memory address of the char on display
            ;
            PUSH    IX
            ; IX  used to index the font table
            ;
            LD      (CHARTODRAW),A; ; Save the character to draw

            CP      13              ; Is it a newline????
            JR      Z,NEWLINE

            LD      HL, LINESTARTS  ; Address of 'line start' table

            LD      A, (CURSORY)    ; Add on 2*cursory
            SLA     A
            ADD     A,L
            LD      L,A
            JR      NC, NOINCY
            INC     H

NOINCY      LD      C, (HL)        ; Load the start of the line into BC
            INC     HL
            LD      B, (HL)

            LD      A, (CURSORX)  ; Add on cursorX
            ADD     A,C
            LD      C,A
            JR      NC, NOINCX
            INC     B

            ; Find which font character to show
NOINCX      LD      IX, FONT       ; default to unknown character
            LD      A,(CHARTODRAW) ; Load the character            
            CP      FIRSTCHAR      ; Is it greater then the first char
            JR      C,  NOCHAR     ; Nope - put the default character
            CP      LASTCHAR+1     ; Is it less than the last char+1
            JR      NC, NOCHAR     ; Nope - put the default

            SUB     FIRSTCHAR-1    ; Calc entry in the table
            LD      L,A            ; Put the font table index into HL 
            SUB     A              ; Zero out H
            LD      H,A

            SLA     L              ; Multiply HL by 8
            RL      H
            SLA     L
            RL      H
            SLA     L
            RL      H

            LD      A, FONT&0FFH
            ADD     A,L
            LD      L,A

            LD      A, FONT/256
            ADC     A,H
            LD      H,A

            LD      (TEMP),HL
            LD      IX,(TEMP)

            LD      (TEMP),HL      ; Put it in reg that the font 
            LD      IX,(TEMP)      ; is indexed from

            ; Place the character on the screen, one line at a time
NOCHAR      LD      H,8            ; How many character lines to place?

NEXTSCANLNE LD      A,(IX)         ; Place a line of character into 
            LD      (BC),A         ; The video memory

            INC     IX             ; Move on to the next line of font
            LD      A,C            ; Move bc to the next row
            ADD     A,BYTESPERLINE           
	    LD      C,A
            JR      NC, NOINLINC   
	    INC     B

NOINLINC    DEC     H              ; Have we updated all lines???
            JR      NZ,NEXTSCANLNE ; Nope - do the next one??

            ; Update the cursor position
            LD      A,(CURSORX)

            CP      CHARSPERLINE-1; Last cursor place on the line?
            JR      Z,NEWLINE     ; Yes - we have to do a newline

            INC     A             ; Otherwise move the cursor across one char
            LD      (CURSORX),A   ; and exit
            JR      DLEAVE

            ; Move the cursor onto a fresh line, scrolling if needed
NEWLINE     SUB     A              ; Move cursor back to start of the line
            LD      (CURSORX),A   

            ; See if we need to scroll
            LD      A,(CURSORY)    ; Last line ?
            CP      LINEONSCREEN-1 ; Are we in the last line
            JR      Z,SCROLL       ; Yes - scroll instead
            INC     A              ; No, Increment the line we are on
            LD      (CURSORY),A    ; and then leave

DLEAVE      LD      A,(CHARTODRAW) ; Restore the character to A
            POP     IX             ; For the caller. It may want to
            POP     BC             ; Do something with it
            POP     HL
            RET

SCROLL      ; The code to scroll the line goes here!
            PUSH    IY         ; IY is the only one not restored on exit
            LD      IX,8000H   ; The start of the first line
            LD      IY,80F0H   ; The start of the second line

            LD      B,LINEONSCREEN-1 ; Counter - Scroll all but last line
EACHLINE    LD      C,BYTESPERLINE * LINESPERROW  ; How many bytes per line
PERLINE     LD      A, (IY)    ; Load from next line
            LD      (IX),A     ; Save in the line
            INC     IX         ; Move on to next bytes
            INC     IY
            DEC     C
            JR      NZ,PERLINE ; Still more on this line?
            DEC     B          
            JR      NZ,EACHLINE ;Another line to go?

            ; Clear off the last line
            LD      C,BYTESPERLINE * LINESPERROW ; Loop counter
            SUB     A                            ; Clear out A
CLEARLOOP   LD      (IX),A                       ; Empty one byte
            INC     IX                           ; Increment the pointer
            DEC     C                            ; Dec the counter
            JR      NZ, CLEARLOOP                ; need to do it again?

            POP     IY      
            JR      DLEAVE
;============ End of code =============
	    .nocodes
MULTICHARS  .DB 00

WELCOMEMSG  .Text  "ZQ700 Terminal program"
            .DB     10
            .DB     13
            .Text  "Written by mafield@the.net.nz"
            .DB     10
            .DB     13
            .DB     00
;
CHARTOSEND    .DB     00
;
CHARTODRAW    .DB     00
 	      .DB     00  ; This must be zero!
;
; Order is important!

KEYDOWN     .DB     00
KEYSELMASK  ; Accessed as a double byte
KEYSEL      .DB     00
KEYMASK     .DB     00

TEMP        .DB 0,0

CURSORY     .DB 00H
CURSORX     .DB 00H


LINESTARTS  .DB 00H
            .DB 80H

            .DB 0F0H
            .DB 80H

            .DB 0E0H
            .DB 81H

            .DB 0D0H ; Line 4
            .DB 82H

            .DB 0C0H
            .DB 83H

            .DB 0B0H
            .DB 84H

            .DB 0A0H
            .DB 85H

            .DB 090H ; Line 8
            .DB 86H

            .DB 080H
            .DB 87H

            .DB 70H ; Line 10
            .DB 88H

FONT        .DB 01FH, 011H, 011H, 011H, 011H, 011H, 01FH, 000H ; unknown
            .DB 000H, 000H, 000H, 000H, 000H, 000H, 000H, 000H ; ' ' 32
            .DB 004H, 004H, 004H, 004H, 000H, 000H, 004H, 000H ; '!' 33
            .DB 00AH, 00AH, 000H, 000H, 000H, 000H, 000H, 000H ; '"' 34
            .DB 000H, 00AH, 01FH, 00AH, 01FH, 00AH, 000H, 000H ; '#' 35
            .DB 004H, 00EH, 005H, 00EH, 014H, 00EH, 004H, 000H ; '$' 36

            .DB 003H, 013H, 008H, 004H, 002H, 019H, 018H, 000H ; '%' 37
            .DB 006H, 009H, 009H, 006H, 015H, 009H, 016H, 000H ; '&' 38
            .DB 004H, 004H, 004H, 000H, 000H, 000H, 000H, 000H ; ''' 39
            .DB 008H, 004H, 002H, 002H, 002H, 004H, 008H, 000H ; '(' 40
            .DB 002H, 004H, 008H, 008H, 008H, 004H, 002H, 000H ; ')' 41
            .DB 000H, 004H, 004H, 01FH, 00EH, 00AH, 000H, 000H ; '*' 42
            .DB 000H, 004H, 004H, 01FH, 004H, 004H, 000H, 000H ; '+' 43
            .DB 000H, 000H, 000H, 000H, 000H, 006H, 006H, 002H ; ',' 44
            .DB 000H, 000H, 000H, 00EH, 000H, 000H, 000H, 000H ; '-' 45
            .DB 000H, 000H, 000H, 000H, 000H, 006H, 006H, 000H ; '.' 46
            .DB 010H, 008H, 008H, 004H, 002H, 002H, 001H, 000H ; '/' 47

            .DB 00EH, 011H, 011H, 015H, 011H, 011H, 00EH, 000H ; '0' 48
            .DB 004H, 006H, 004H, 004H, 004H, 004H, 00EH, 000H ; '1' 49
            .DB 00EH, 011H, 010H, 008H, 004H, 002H, 01FH, 000H ; '2' 50
            .DB 00EH, 011H, 010H, 00EH, 010H, 011H, 00EH, 000H ; '3' 51
            .DB 008H, 00CH, 00AH, 01FH, 008H, 008H, 01CH, 000H ; '4' 52
            .DB 01FH, 001H, 001H, 00FH, 010H, 011H, 00EH, 000H ; '5' 53
            .DB 01CH, 002H, 001H, 00FH, 011H, 011H, 00EH, 000H ; '6' 54
            .DB 01FH, 010H, 010H, 008H, 004H, 002H, 002H, 000H ; '7' 55
            .DB 00EH, 011H, 011H, 00EH, 011H, 011H, 00EH, 000H ; '8' 56
            .DB 00EH, 011H, 011H, 01EH, 010H, 008H, 007H, 000H ; '9' 57
            .DB 000H, 000H, 006H, 006H, 000H, 006H, 006H, 000H ; ':' 58
            .DB 000H, 000H, 006H, 006H, 000H, 006H, 006H, 002H ; ';' 59
            .DB 000H, 018H, 006H, 001H, 006H, 018H, 000H, 000H ; '<' 60
            .DB 000H, 000H, 01FH, 000H, 01FH, 000H, 000H, 000H ; '=' 61
            .DB 000H, 003H, 00CH, 010H, 00CH, 003H, 000H, 000H ; '>' 62
            .DB 00EH, 011H, 008H, 004H, 004H, 000H, 004H, 000H ; '?' 63

            .DB 00EH, 011H, 01DH, 01BH, 01DH, 001H, 00EH, 000H ; '@' 64
            .DB 00EH, 011H, 011H, 01FH, 011H, 011H, 011H, 000H ; 'A' 65
            .DB 00FH, 011H, 011H, 00FH, 011H, 011H, 00FH, 000H ; 'B'
            .DB 00EH, 011H, 001H, 001H, 001H, 011H, 00EH, 000H ; 'C'
            .DB 00FH, 011H, 011H, 011H, 011H, 011H, 00FH, 000H ; 'D'
            .DB 01FH, 001H, 001H, 00FH, 001H, 001H, 01FH, 000H ; 'E'
            .DB 01FH, 001H, 001H, 00FH, 001H, 001H, 001H, 000H ; 'F' 70
            .DB 00EH, 011H, 001H, 001H, 019H, 011H, 00EH, 000H ; 'G' 
            .DB 011H, 011H, 011H, 01FH, 011H, 011H, 011H, 000H ; 'H' 
            .DB 00EH, 004H, 004H, 004H, 004H, 004H, 00EH, 000H ; 'I' 
            .DB 010H, 010H, 010H, 010H, 010H, 011H, 00EH, 000H ; 'J'
            .DB 011H, 009H, 005H, 003H, 005H, 009H, 011H, 000H ; 'K' 75
            .DB 001H, 001H, 001H, 001H, 001H, 001H, 01FH, 000H ; 'L'  
            .DB 011H, 01BH, 015H, 011H, 011H, 011H, 011H, 000H ; 'M'  
            .DB 011H, 013H, 015H, 015H, 015H, 019H, 011H, 000H ; 'N'  
            .DB 00EH, 011H, 011H, 011H, 011H, 011H, 00EH, 000H ; 'O'
            .DB 00FH, 011H, 011H, 00FH, 001H, 001H, 001H, 000H ; 'P' 80
            .DB 00EH, 011H, 011H, 011H, 011H, 015H, 00EH, 008H ; 'Q'
            .DB 00FH, 011H, 011H, 00FH, 005H, 009H, 011H, 000H ; 'R'
            .DB 00EH, 011H, 001H, 00EH, 010H, 011H, 00EH, 000H ; 'S' 
            .DB 01FH, 004H, 004H, 004H, 004H, 004H, 004H, 000H ; 'T' 
            .DB 011H, 011H, 011H, 011H, 011H, 011H, 00EH, 000H ; 'U' 85 
            .DB 011H, 011H, 011H, 011H, 00AH, 00AH, 004H, 000H ; 'V'  
            .DB 011H, 011H, 011H, 011H, 015H, 01BH, 011H, 000H ; 'W'  
            .DB 011H, 011H, 00AH, 004H, 00AH, 011H, 011H, 000H ; 'X'  
            .DB 011H, 011H, 011H, 00AH, 004H, 004H, 004H, 000H ; 'Y'  
            .DB 01EH, 010H, 008H, 004H, 002H, 001H, 01EH, 000H ; 'Z' 90 
            .DB 00EH, 002H, 002H, 002H, 002H, 002H, 00EH, 000H ; '[' 91
            .DB 001H, 002H, 002H, 004H, 008H, 008H, 010H, 000H ; '\' 92
            .DB 00EH, 008H, 008H, 008H, 008H, 008H, 00EH, 000H ; ']' 93
            .DB 004H, 00aH, 011H, 000H, 000H, 000H, 000H, 000H ; '^' 64
            .DB 000H, 000H, 000H, 000H, 000H, 000H, 01FH, 000H ; '_' 95

            .DB 003H, 002H, 002H, 000H, 000H, 000H, 000H, 000H ; '`' 97
            .DB 000H, 000H, 00EH, 010H, 01EH, 011H, 01EH, 000H ; 'a' 98
            .DB 001H, 001H, 00FH, 011H, 011H, 011H, 00FH, 000H ; 'b' 99
            .DB 000H, 000H, 00EH, 011H, 001H, 011H, 00EH, 000H ; 'c' 100
            .DB 010H, 010H, 01eH, 011H, 011H, 011H, 01eH, 000H ; 'd' 101
            .DB 000H, 000H, 00EH, 011H, 01FH, 001H, 01EH, 000H ; 'e' 112
            .DB 00CH, 002H, 002H, 007H, 002H, 002H, 002H, 000H ; 'f' 103
            .DB 000H, 000H, 01eH, 011H, 011H, 01eH, 010H, 00EH ; 'g' 104
            .DB 001H, 001H, 001H, 00FH, 011H, 011H, 011H, 000H ; 'h' 105
            .DB 000H, 004H, 000H, 006H, 004H, 004H, 00eH, 000H ; 'i' 106
            .DB 000H, 008H, 000H, 008H, 008H, 008H, 008H, 006H ; 'j' 107
            .DB 001H, 001H, 009H, 005H, 003H, 005H, 009H, 000H ; 'k' 108
            .DB 002H, 002H, 002H, 002H, 002H, 002H, 002H, 000H ; 'l' 109 
            .DB 000H, 000H, 00AH, 015H, 015H, 015H, 015H, 000H ; 'm' 110
            .DB 000H, 000H, 00FH, 011H, 011H, 011H, 011H, 000H ; 'n' 111
            .DB 000H, 000H, 00EH, 011H, 011H, 011H, 00EH, 000H ; 'o' 112
            .DB 000H, 000H, 00FH, 011H, 011H, 00FH, 001H, 001H ; 'p' 113
            .DB 000H, 000H, 01eH, 011H, 011H, 01eH, 010H, 010H ; 'q' 114
            .DB 000H, 000H, 00DH, 003H, 001H, 001H, 001H, 000H ; 'r' 115
            .DB 000H, 000H, 00EH, 001H, 00EH, 010H, 00EH, 000H ; 's' 116
            .DB 002H, 007H, 002H, 002H, 002H, 002H, 004H, 000H ; 't' 117
            .DB 000H, 000H, 011H, 011H, 011H, 011H, 01EH, 000H ; 'u' 118
            .DB 000H, 000H, 011H, 011H, 00AH, 00AH, 004H, 000H ; 'v' 119 
            .DB 000H, 000H, 011H, 011H, 015H, 01BH, 011H, 000H ; 'w' 120 
            .DB 000H, 000H, 011H, 00AH, 004H, 00AH, 011H, 000H ; 'x' 121 
            .DB 000H, 000H, 011H, 011H, 011H, 01eH, 010H, 00EH ; 'y' 122
            .DB 000H, 000H, 01EH, 008H, 004H, 002H, 01EH, 000H ; 'z' 123 
            .DB 008H, 004H, 004H, 002H, 004H, 004H, 008H, 000H ; '{' 124
            .DB 004H, 004H, 004H, 004H, 004H, 004H, 004H, 000H ; '|' 125
            .DB 002H, 004H, 004H, 008H, 004H, 004H, 002H, 000H ; '}' 126
            .END


