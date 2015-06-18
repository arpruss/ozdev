/*
  This module has misc. general purpose routines that are used throughout
  the entire chess program.
*/


#define HEADER
#include "arvar.c"

static char blackmates[]="Blac\xEB""mates";
static char whitemates[]="Whit\xE5""mates";

void findpieces(void)
/* find location of the king, the number of pawns, and the number of
   pawns on each column */
{ register int x;
pawncnt[WHITE]=pawncnt[BLACK]=pawncnt[NONE]=0;
kingloc[WHITE]=kingloc[BLACK]=kingloc[NONE]=-1;
memset(pawncol,0,sizeof pawncol);
/* for (x=7;x>=0;x--) pawncol[WHITE][x]=pawncol[BLACK][x]=pawncol[NONE][x]=0; */

for (x=63;x>=0;x--) {
  if (board[x]==KING) kingloc[brdcolor[x]]=x;
  if (board[x]==PAWN)
    { pawncnt[brdcolor[x]]++;
      pawncol[brdcolor[x]][(x & 7)]++;
    }
  }
mtl[WHITE]=mtl[BLACK]=mtl[NONE]=0;
for (x=63;x>=0;x--) mtl[brdcolor[x]]+=pieceval[board[x]];
}

void mvenglish(movetype *mv,char *st)
/*translate the move into english*/
{static char s[10];static uchar f,t;

f=mv->f;t=mv->t;
if (mv->flags & CASTLE)
  {if (t>f) strcpy(st,"O-\xCF""");
   else     strcpy(st,"O-O-\xCF""");
   return;
  }
s[0]=('A'+getcol(f));s[1]=('1'+getrow(f));
s[2]='-';if (mv->flags & CAPTURE) s[2]='x';
s[3]=('A'+getcol(t));s[4]=('1'+getrow(t));
s[5]=name[mv->flags & 7];s[6]=0;
if (mv->flags & ENPASSANT) {s[5]='e';s[6]='p';}
s[7]=0;
strcpy(st,s);
}

void checkstatus(uchar tomove)
/* this calls findpiece and then checks to make sure both kings exist.
   if not, then the global flag mate is set */
/* This needs some work.  It should also check the number of legal
   moves the side to move can make (in case of stalemate).  -- FIXED, ARP */
{ static uchar side;
  movetype buffer[MAXPLYMVS];
mate=FALSE;findpieces();status[0]=0;
if ((kingloc[WHITE]<0) || (kingloc[BLACK]<0))
{
   mate=TRUE;
   if (kingloc[WHITE]<0) side=BLACK;
     else side=WHITE;
/*   if (human==BLACK) side=chngcolor(side); */
   if (side==WHITE) strcpy(status,whitemates);
     else strcpy(status,blackmates);
   bothsides=FALSE;
} else if(sqatakd(kingloc[chngcolor(tomove)],tomove))
{
    mate=TRUE;
    if(tomove==WHITE)
        strcpy(status,whitemates);
    else
        strcpy(status,blackmates);
    bothsides=FALSE;
}
else if(buffer==genlegal(buffer,tomove) || repetition()>2)
  {
     if(sqatakd(kingloc[tomove],chngcolor(tomove)))
     {
        mate=TRUE;
        if(tomove==BLACK)
            strcpy(status,whitemates);
        else
            strcpy(status,blackmates);
    }
    else
    {
        draw=TRUE;
        strcpy(status,"Draw\xEE""game");
    }
    bothsides=FALSE;
    side=chngcolor(tomove);
  }
}

bool mveq(movetype *mv1,movetype *mv2)
{
if (mv1->f != mv2->f) return(FALSE);
if (mv1->t != mv2->t) return(FALSE);
if ((mv1->flags & 0xff) != (mv2->flags & 0xff)) return(FALSE);
return(TRUE);
}

#if 0
int distance(int x,int y)
/* The distance between two squares.  Only the greatest of the distances
   of the rows or the columns is the 'distance'.  */
{ static int a,b;
  a=abs(getcol(x)-getcol(y));
  b=abs(getrow(x)-getrow(y));
  if (a > b) return(a); else return(b);
}
#endif
