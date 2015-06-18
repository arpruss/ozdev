/*
  Theis contains various search support routines
*/


#define HEADER
#include "arvar.c"

int setmate(movetype *move,uchar side,uchar depth)
/* set the score for a mate to an extremely high value.  We adjust the value
   by subbing depth from BIGNUM so that if we find a mate during a capture
   the shorter mates will be better.               */
{
    static int score;
    static uchar klw;
    score=MATEVAL-depth;
    if ((klw=(kingloc[WHITE]<0)) || kingloc[BLACK]<0)
    {
        if(klw) score=-score;
        if(side==BLACK) score=-score;
    }
    move->flags|=CHECKMATE | EXACT;
    return(score);
}

int repetition(void)
/* detect draw by repetition and the 50 move rule */
{static int i;
static direct int rep,cnt;
static tiny b[64];
/* static direct tiny *bptr; */
register gametype *gp;

if (movenum<=game50+3) return(0);
rep=cnt=0;gp=&gamehist[movenum];
/* for (bptr=&b[64];bptr>=&b[0];*(--bptr)=0); */
memset(b,0,64);

for (i=movenum;i>game50;i--)
   {if ((++b[gp->f])==0) cnt--; else cnt++;
    if ((--b[gp->t])==0) cnt--; else cnt++;
    if (cnt==0) rep++;
    gp--;
   }

if ((movenum-game50)>99) return 50; /* 50 move rule */
return(rep);
}

/*
void pvsenglish(movetype *pvs)
{static int x,y;static char s[9];

pvsmsg[0]='\0';
y=1;
while (y < 12)
   {if (pvs[y].f >= 0) {mvenglish(&pvs[y],s);strcat(pvsmsg,s);}
    else break;
    y++;
   }
}
*/

#ifdef HISTORY

#if 0
void scalehistory(void)
/* Reduce the history[] by dividing by 256 so it won't overflow.
   This is preferable to clearing it because it still has information
   in it that can be used to order the moves */
{
static int x;
for (x=4095;x>=0;x--) writehistory(x,readhistory(x)>>8);
}
#endif

#endif

void searchtime(int research,int depth,int bstscore,
                int lowbound,int avgscore,int zwindow)
/* The adjustments are based on how the value of the move just searched */
/* compares to the best score we've found so far.                       */
/* It never reduces the alloted search time for a move.  I guess I
   should add something for the occasional 'obvious' move.              */
/* I need to be carefull not to overflow the extratime                  */
{static int worse;

if (research==-1)                   extratime=responsetime*5;
else if (research==1)               extratime=0;
else {worse=bstscore-avgscore;
      if (level==1)                 extratime=BIGNUM;    /* FIND ONE */
      else if (bstscore < lowbound) extratime=responsetime*3;
      else if (worse > -zwindow)    extratime=0;
      else if (worse > -3*zwindow)  extratime=responsetime;
      else if (worse > -9*zwindow)  extratime=responsetime*3;
      else                          extratime=20+responsetime*5;
     }
}

void selected(void)
/*we've finally selected a move, now do it!*/
{

elaptime();
/*
if (breakpress)
   { strcpy(msg,"Searc\xE8""interupted");pvsmsg[0]=status[0]=0;
     bothsides=FALSE;Bell();return;
   } */

findpieces();/* pvsenglish(pvs); */
checkstatus(tomove);
if (! mate)
{
    processmove(&pvs[1]);
    if(!draw)
        {strcpy(msg,"M\xF9""move\xBA");
         mvenglish(&pvs[1],msg+7);}
}
}

void unflag(movetype *mvlist,movetype *last,bool reset)
{ /* static int x,y;*/
  register movetype *mv;
for (mv=mvlist+1;mv<=last;mv++)
   if (reset)
     {mv->flags &= ~ (FULLSRCH | FULLEVAL | STANDPAT | EXACT);
      mv->score >>=3;
      if (mveq(mv,&pvs[1])) mv->score=BIGNUM;
     }
   else if (!(mv->flags & EXACT))
           mv->flags &= ~ (FULLSRCH | FULLEVAL | STANDPAT);
sort(mvlist,last);
}


