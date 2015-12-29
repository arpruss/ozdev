/*
  This handles the help command and also allows the user to set up a
  board position
*/

#define HEADER
#include "arvar.c"

#ifndef OZ7XX
void setboard(void)
/*this allows the user to set up their own board*/
{int row,col,piece,color;
 bool done,changed;
 movetype fakemv;bool check;

status[0]=0;changed=FALSE;fakemv.flags=0;
do {
   printboard();printf("%s\n",status);status[0]=0;
   printf("Enter pieces like a4wk a4e\n");
   printf("Cmds are clear, new, done\n");
   printf("Done to return: ");
   readcmd();
   if (cmdis("CLEAR")) {clearboard();changed=TRUE;}
   else if (cmdis("EVAL"))
           {findpieces();
            estscore=evalu8(&fakemv,0,tomove,-BIGNUM,BIGNUM,1,&check,FALSE);}
   else if (cmdis("NEW")) {newboard();changed=TRUE;}
   else if ((strlen(cmdline)==4) || (strlen(cmdline)==3))
         {col=(cmdline[0])-'A';row=(cmdline[1])-'1';
          color=NONE;piece=EMPTY;
          if (cmdline[2]=='W') color=WHITE;
          if (cmdline[2]=='B') color=BLACK;
          if (cmdline[3]=='P') piece=PAWN;
          if (cmdline[3]=='N') piece=KNIGHT;
          if (cmdline[3]=='B') piece=BISHOP;
          if (cmdline[3]=='R') piece=ROOK;
          if (cmdline[3]=='Q') piece=QUEEN;
          if (cmdline[3]=='K') piece=KING;
          if ((color==NONE) && (piece!=EMPTY)) piece=EMPTY;
          if ((color!=NONE) && (piece==EMPTY)) color=NONE;
          if ( (row>=0) && (row<=7) && (col>=0) && (col<=7) )
            {board[row*8+col]=piece;brdcolor[row*8+col]=color;
             changed=TRUE;};
        };
   findpieces();
   done=cmdis("DONE") || cmdis("END") || cmdis("EXIT") ||
        cmdis("QUIT") || cmdis("STOP");
   if ( (kingloc[WHITE] < 0) || (kingloc[BLACK] < 0) )
      {strcpy(status,"You need both kings!");Bell();done=FALSE;}
} while (! done);
if (changed) newgame();
}

static char * helpmsg[]=
/*    1234567890123456789012345678901234567890*/
    {"black    : set black to play\n",
     "both     : computer plays both sides\n",
     "enter    : enter a sequence of moves\n",
     "eval     : evaluate current position\n",
     "go       : play current side\n",
     "level    : set play level depth\n",
     "move     : computer moves for side\n",
     "new      : new game\n",
     "play     : play current side\n",
     "reverse  : reverse board display\n",
     "setup    : setup new board position\n",
     "side     : change side to move\n",
     "skip     : change sides and play\n",
     "swap     : change sides of board\n",
     "time     : set play level time in sec\n",
     "undo     : take back a move\n",
     "undraw   : unflag a drawn game\n",
     "unmate   : unflag a mate game\n",
     "white    : set white to play\n",
     "enter moves as e2e4, b1c3, h7h8q\n",
     "press enter to continue:",
     ""};

void help(void)
/* print out the help message */
{int x;

clrscrn();
x=0;
while ((*helpmsg[x])!=0)
  {printf("%s",helpmsg[x]);
   x++;
  };

/* wait until enter is pressed */
readcmd();
}
#else
void setboard(void){}
void help(void){}
#endif
