;
;  WRITBYTE.ASM  PROGRAM 
;       05/08/2000
;       Rob Jahnke 
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
;
; Program Description:
;
;    CAUTION:  THIS PROGRAM COULD DESTORY THE WIZARD!
;
;    Writes FCh to memory location 50:0000
;
;    This will allow a test to see if we can write a byte in "unused"
;    Memory and then recover with a RESTORE
;
;    Wizard was backed up first, then byte was written.
;    RESTORE replaced the byte with the original FFh
;
;
;    Use this program for your own education and enjoyment
;    There are no warrantees, expressed or implied so use at your own risk.
;    This program is unsupported freeware.
;       by Rob Jahnke

	.nocodes
#define TITLE	.Text  	"Stack = FFFC"			   			; 14 Chrs max
#define	NAME	.Text	"Bxxx0000"			   				; B + 7 Chrs
#define	DESCRIP	.Text	"Reserves 3 bytes at top of RAM."  	; use \r\n for cr lf
	.nolist
#include wzdhdr0.asm

SERSTAT     .EQU    45H
SIN         .EQU    40H
SOUT        .EQU    40H
;
;
START       DI                  ;Disable interupts

			PUSH    AF          ;Save all the registers
            PUSH    BC
            PUSH    DE
            PUSH    HL

			LD	    HL,MSG1
            CALL    SENDMSG     ;Send "MEMORY WRITE IN PROGRESS!"
            
            IN      A,(01H)     ;Save the Memory Management Regs in BC
            LD      C,A         ; 
            IN      A,(02H)     ; 
            LD      B,A

            PUSH    BC          ; Put the saved regs on the stack

            LD      A,04CH      ; Set the page registers so that
            OUT     (01H),A     ; z:8000H is mapped to 50h in a-space
            LD      A,00H       ; 50h-04h=4C for correct map to
            OUT     (02H),A 	; z:8000h                     			

;			LD A,0FFH 			; RESET FLASH CONTROLLER
;			LD (8000H),A
;	DON'T		LD A,60H 			; CLEAR ALL LOCK BITS CMD - FIRST BYTE
;	 DO		LD (8000H),A
;	THIS!!!		LD A,0D0H 			; CLEAR ALL LOCK BITS CMD - SECOND BYTE
;			LD (8000H),A
;			CALL WAIT_FOR_READY

			LD A,0FFH 			; RESET FLASH CONTROLLER
			LD (8000H),A
			LD A,40H			; WRITE BYTE COMMAND
			LD (8000H),A
			LD A,0FCh			; WRITE VALUE - NOTE!: ONLY WRITE'S ZERO BITS
			LD (8000H),A
			CALL WAIT_FOR_READY

			LD A,0FFH 			; RESET FLASH CONTROLLER
			LD (8000H), A  	

	        LD      HL,MSG3     ;Send "MEMORY WRITE COMPLETE!"
            CALL    SENDMSG
            

EXIT        POP     BC          ;Get the saved MMU regs          
            LD      A,C         ;Restore the MMU regs
            OUT     (01H),A
            LD      A,B
            OUT     (02H),A       

            POP     HL          ;Restore the regs
            POP     DE
            POP     BC
            POP     AF
 			EI                  ;Enable Interupts
            RET
;
; WAIT_FOR_READY *******************************************************
; Waits until the Flash WSM becomes ready
; USES:    A
;
WAIT_FOR_READY
			LD A,0FFh			; RESET FLASH CONTROLLER
			LD (8000h),A
			LD A,70h			; READ STATUS COMMAND
			LD (8000h),A
			LD A,(8000h)		; READ THE STATUS BYTE
			AND 	80h			; Test for READY
			JR Z,WAIT_FOR_READY
			RET
;
;
; Subroutine to send a serial msg:
; ON ENTRY:     HL points to MSG
;               Message is terminated with 00
; USES:         HL, A
;  
SENDMSG     LD      A,(HL)      ;Get Data Byte
	    	OR      A		;Check for 00 Terminator
            RET     Z           ;All done - Rtn with A&F = 0	         
CHKCTS      IN      A,(SERSTAT) ;Check CLEAR TO SEND
            AND     20H
            JR      Z,CHKCTS    ;Wait until clear to send
            LD      A,(HL)      ;Get Data Byte
            OUT     (SOUT),A    ;Send it to the Serial Out Port
            INC     HL          ;Point to next Data Byte
            JR      SENDMSG     ;Go back for next byte
;
; Subroutine to check for the escape key
;
; USES:         A,F
; RETURN:       A=0 if no ESC key
;               A=1 if ESC key
;               Z Flag Set is ESC key is not pushed
;
CHKESC      LD      A,1
            OUT     (17),A      ;Select first row of keys
            IN      A,(16)      ;Get the First row of data
            AND     01H         ;Check for ESC
            RET                 ;Return with NZ flag set
;
; Subroutine to get a serial character
;
;  USES:        A,F
;  RETURN:      Character in A
;  Waits Until a character comes in or ESC key is pressed
;  IF ESC key is pressed , A=1 and NZ Flag is set
;
GETCHR      CALL    CHKESC      ;
            RET     NZ          ;Return if ESC key is pressed
            IN      A,(SERSTAT) ;Check Serial Status
            AND     1
            JR      Z,GETCHR    ;Stay in loop until something is input
            IN      A,(SIN)     ;Get the Serial Character into A
            CP      A           ;Set Z Flag to indicate no ESC key
            OUT     (SOUT),A    ;Echo Character
            RET                 ;and Return
;
	    .nocodes
;
MSG1        .Text   "MEMORY WRITE IN PROGRESS! \r\n"
	    	.DB	    00	
MSG2        .Text   "\r\r\nThank You! I'm Outa Here!\r\n"
            .DB     00
MSG3        .Text   "MEMORY WRITE COMPLETE! \r\n"
            .DB     00
            .END
