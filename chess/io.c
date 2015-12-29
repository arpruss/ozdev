/*
  This module handles all of the input/output of the chess program.
  No effort is made for a fancy screen display, that would add code and
  increse the effort to port
*/

/* #define BENCHMARK */
#define HEADER
#include "arvar.c"

extern uchar currow,curcol;

#if 0
void entermoves(void)
{ static bool done;

done=FALSE;
while (!done)
  {printboard();
   printf("ENTERMOVES command. DONE to return\n%s\n",msg);
   printf("BLACK DONE UNDO WHITE\n");
   printf("MOVE: ");readcmd();

   if (cmdis("BLACK"))      tomove=BLACK;
   else if (cmdis("DONE"))  done=TRUE;
   else if (cmdis("UNDO"))  undo();
   else if (cmdis("WHITE")) tomove=WHITE;
   else verifymove();
  }
}
#endif

void undo(void)
/* take back 1 move */
{ register gametype *gp;

if (movenum<=0)
  {Bell();strcpy(msg,"No moves to take back");return;}

gp=&gamehist[movenum];
unmakemove();
tomove=gp->color;
draw=mate=FALSE;
msg[0]=/* pvsmsg[0]= */ 0;
}

void inputcommand(void)
/*input a command or move from the human*/
{static bool done;
 static movetype fakemv;
 static bool check;

done=FALSE;quit=FALSE;fakemv.flags=0;
while (! (done || quit))
  {checkstatus();printboard();
/*   if (pvsflag) printf("%s\n",pvsmsg); */
   currow=6; atright();
   if (status[0]) printf("%s\n",status);
   currow=7; atright();
   printf(msg);
   currow=8; atright();
   printf("New Quit\n");
   atright();
   printf("Cmd: ");readcmd();
   if ( (cmdis("QUIT")) || (cmdis("END")) ||
        (cmdis("STOP")) ) quit=TRUE;
   else if (cmdis("TIME"))
        {   currow=9;atright(); printf("Time:    \8\8\8");readcmd();
         responsetime=atoi(cmdline);tcontrol=TRUE;
/* limit search time to 6500 seconds (or 108 minutes, or 1 hr, 48 min) */
         if (responsetime>6500)   responsetime=6500;
         if (responsetime<1)      responsetime=1;}
   else if (cmdis("LEVEL"))
        {   currow=9;atright(); printf("Depth:    \8\8\8");readcmd();
         playlevel=atoi(cmdline);tcontrol=FALSE;
         if (playlevel>MAXLEVEL) playlevel=MAXLEVEL;
         if (playlevel<1)        playlevel=1;}
   else if (cmdis("BLACK"))    tomove=BLACK;
   else if (cmdis("BOTH"))     {bothsides=TRUE;done=TRUE;}
#ifdef BENCHMARK
   else if (cmdis("CAPTIME"))  captime();
#endif
#ifdef DEBUG
   else if (cmdis("STACK"))    { extern unsigned minstack;
                                 currow=0; curcol=0;
                                 printf("%d",minstack);
                                 ozgetch(); }
#endif
#if 0
   else if (cmdis("ENTER"))    entermoves();
#endif
   else if (cmdis("EVAL"))     {findpieces();
            estscore=evalu8(&fakemv,0,tomove,-BIGNUM,BIGNUM,1,&check,FALSE);}
#ifdef BENCHMARK
   else if (cmdis("EVALTIME")) evaltime();
   else if (cmdis("GENTIME"))  gentime();
#endif
   else if (cmdis("GO"))       done=TRUE;
/*   else if (cmdis("HELP"))     help(); */
   else if (cmdis("MOVE"))     selectmove(tomove);
   else if (cmdis("NEW"))      initgame();
   else if (cmdis("PLAY"))     done=TRUE;
/*   else if (cmdis("PVSY"))     pvsflag=TRUE;
   else if (cmdis("PVSN"))     pvsflag=FALSE; */
   else if (cmdis("QSEARCHY")) qsearch=TRUE;
   else if (cmdis("QSEARCHN")) qsearch=FALSE;
   else if (cmdis("REVERSE"))  reverse=!reverse;
/*   else if (cmdis("SETUP"))    setboard(); */
   else if (cmdis("SIDE"))     tomove=chngcolor(tomove);
   else if (cmdis("SKIP"))     {done=TRUE;tomove=chngcolor(tomove);}
   else if (cmdis("SWAP"))     {human=chngcolor(human);
                                computer=chngcolor(computer);
                                tomove=chngcolor(tomove);}
   else if (cmdis("UNDO"))     undo();
   else if (cmdis("UNDRAW"))   draw=FALSE;
   else if (cmdis("UNMATE"))   mate=FALSE;
   else if (cmdis("WHITE"))    tomove=WHITE;
   else if (cmdis("SNAP"))     { ozsnap(); ozsnap(); }
   else  done=verifymove();
  };
checkstatus();
}

