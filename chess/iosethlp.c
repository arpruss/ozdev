/*
  This handles the help command and also allows the user to set up a
  board position
*/

#define HEADER
#include "arvar.c"

#include <ozinput.h>

#define SETUP /* */
#define HELP /* */

void setboard(void)
/*this allows the user to set up their own board*/
{
#ifdef SETUP
 extern uchar currow;
 static uchar row,col,piece,color;
 static bool done,changed;

status[0]=0;changed=FALSE;
do {
   printboard();
   currow=6; atright();
   if (status[0])
   {
      termputs(status);
      *status=0;
   }
   currow=7; atright();
   printf("Clea\xF2""Ne\xF7""Done\n");
   atright();
   printf("XYC\xD0""XYE\n");
   atright();
   printf("Cmd\xBA""");
   movepieces=doenter=1;
   readcmd();
   movepieces=doenter=0;
   if (cmdis("CL")) {clearboard();changed=TRUE;}
   else if (cmdis("NEW")) {newboard();changed=TRUE;}
   else if ((strlen(cmdline)==4) || (strlen(cmdline)==3))
		 {col=(cmdline[0])-'A';row=(cmdline[1])-'1';
		  color=NONE;piece=EMPTY;
          switch(cmdline[2])
          {
            case 'W': color=WHITE; break;
            case 'B': color=BLACK; break;
          }
          switch(cmdline[3])
          {
            case 'P': piece=PAWN; break;
            case 'N': piece=KNIGHT; break;
            case 'B': piece=BISHOP; break;
            case 'R': piece=ROOK; break;
            case 'Q': piece=QUEEN; break;
            case 'K': piece=KING; break;
          }
		  if ((color==NONE) && (piece!=EMPTY)) piece=EMPTY;
		  if ((color!=NONE) && (piece==EMPTY)) color=NONE;
          if ( (row<=7) && (col<=7) )
			{board[row*8+col]=piece;brdcolor[row*8+col]=color;
			 changed=TRUE;};
		};
   findpieces();
   done=cmdis("DO") || cmdis("QUIT");
   if ( (kingloc[WHITE] < 0) || (kingloc[BLACK] < 0) )
      {strcpy(status,"Nee\xE4""tw\xEF""kings!");Bell();done=FALSE;}
} while (! done);
if (changed) newgame();
#endif
}

#ifdef HELP
static char * helpmsg[]=
/*	  12345678901234567890123456789 */
    {"BLACK\3blac\xEB""t\xEF""play*",
     "BLANK\3toggl\xE5""blanking",
     "BOARD\3boar\xE4""size",
     "BOTH\4O\xDA""play\xF3""bot\xE8""sides",
     "FONT\4ches\xF3""font",
     "GO/PLA\xD9""O\xDA""make\xF3""thi\xF3""move",
     "HELP\4Thi\xF3""hel\xF0""screen",
     "LEVEL\3se\xF4""pla\xF9""leve\xEC""depth",
     "LOAD\4loa\xE4""fro\xED""memory",
     "MOVE\4O\xDA""play\xF3""thi\xF3""side",
     "NEW\5ne\xF7""game",
     "PLAY\4pla\xF9""curren\xF4""sid\xE5""now",
     "QUIT\4exit",
     "QSEARCHY/QSEARCHN",
     "\5quiescenc\xE5""searc\xE8""ON/OFF",
     "REVERS\xC5""revers\xE5""boar\xE4""display",
     "SAVE\4sav\xE5""t\xEF""memory",
     "SETUP\3setu\xF0""board*",
     "SIDE\4chang\xE5""sid\xE5""t\xEF""move*",
     "SKIP\4ski\xF0""move*",
     "SNAP\4sen\xE4""imag\xE5""t\xEF""PC",
     "SWAP\4chang\xE5""side\xF3""wit\xE8""OZ",
     "TIME\4se\xF4""O\xDA""thinkin\xE7""time",
     "UNDO\4tak\xE5""bac\xEB""\xE1""move",
     "UNDRAW/UNMATE\2unfla\xE7""game*",
     "WHITE\3se\xF4""whit\xE5""t\xEF""play*",
     "*Ma\xF9""resul\xF4""i\xEE""illega\xEC""game.",
	 "Ente\xF2""move\xF3""a\xF3""E2E4\xAC""H7H8Q,",
     "O-O-O\xAC""o\xF2""us\xE5""`$%\xA6""an\xE4""ENTER."
	};
/*	  12345678901234567890123456789 */
#define HELP_ENTRIES (sizeof helpmsg / sizeof (helpmsg[0]))

void help(void)
/* print out the help message */
{
	extern uchar curcol,currow;
    static uchar pos;
    static uchar i;
    static uchar z;
	pos=0;
	while(1)
	{
		ozcls();
		curcol=currow=0;
        for(z=pos,i=0;i<9 && z<HELP_ENTRIES;i++,z++)
		{
            termputs(helpmsg[z]);
            crlf();
		}
        reversevideo=0xFF;
        termputs("ESC:exit\xAC""`$:browse");
        reversevideo=0;
		switch(ozgetch())
		{
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
				return;
			case KEY_UP:
			case KEY_PAGEUP:
				if(pos>0) pos-=9;
				break;
			case KEY_DOWN:
			case KEY_PAGEDOWN:
                if(z<HELP_ENTRIES) pos+=9;
				break;
		}
	}
}

#else

void help(void){}
#endif
