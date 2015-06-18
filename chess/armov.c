/*
  This module is concerned with making and unmaking the actual moves
*/

#define HEADER
#include "arvar.c"

void processmove(movetype *mv)
{
   register gametype *gp;
   static gametype *gp1;
   if (movenum>=MAXHIST)
   {
        movenum-=SHIFT_HIST_BY;
        gameptr-=SHIFT_HIST_BY;
        shifted_gamehist+=SHIFT_HIST_BY;
        if(game50>SHIFT_HIST_BY) game50-=SHIFT_HIST_BY; else game50=0;
        gp1=gamehist;
        gp=gamehist+SHIFT_HIST_BY;
        for(;gp<gamehist+MAXHIST;gp++,gp1++)
        {
            if(gp->game50>SHIFT_HIST_BY) gp->game50 -= SHIFT_HIST_BY;
              else gp->game50=0;
            memcpy(gp1,gp,sizeof *gp);
        }
   }
if (mv->f >= 0) makemove(mv); /* make sure there is a move! */
checkstatus(chngcolor(tomove));
if (draw || mate) {bothsides=FALSE;}
}

/* these are used in makemove() and unmakemove().  The could just
   as easily be auto vars */
static direct int f,fcol,t,tcol;
static direct int c,oc;
static direct uchar piece,captured;
static direct uchar flags; /* only need lower 8 bits */
static direct int x,y;

void makemove(movetype *_mv)
/* this makes a move and adjusts the material balance, pawn count, king
   location and everything else that changes when you make a move */
{
    register movetype *mv=_mv;
f=mv->f;t=mv->t;fcol=getcol(f);tcol=getcol(t);
c=brdcolor[f];oc=chngcolor(c);
piece=board[f];captured=board[t];flags=mv->flags;

gameptr++;movenum++;
gameptr->f=f;          gameptr->t=t;              gameptr->flags=flags;
gameptr->piece=piece;  gameptr->captured=captured;gameptr->color=c;
gameptr->game50=game50;gameptr->epsquare=epsquare;

/* update 50 move rule index */
if ( (captured) || (piece==PAWN) ||
     (flags & CASTLE) || (flags & ENPASSANT)) game50=movenum;

/* make the move */
board[t]=board[f];brdcolor[t]=c;      /* update the board */
board[f]=EMPTY;   brdcolor[f]=NONE;   /* delete from position */
mvboard[f]++;                         /* increment move count */

/* possible king location change */
if (piece==KING) kingloc[c]=t;
if (captured==KING) kingloc[oc]=-1;

/* a capture */
if (captured) {mtl[oc]-=pieceval[captured];mvboard[t]++;}

/* possible pawn captured */
if (captured==PAWN) {pawncnt[oc]--;pawncol[oc][tcol]--;}

/* possible pawn move */
if (piece==PAWN) {pawncol[c][fcol]--;pawncol[c][tcol]++;}

/* possible pawn promotion upon reaching 8th or 1st rank */
if (flags & PROMOTION)
    {x=flags & 7;
     mtl[c]=mtl[c]+pieceval[x]-PAWNVAL;
     board[t]=x;
     pawncnt[c]--;pawncol[c][tcol]--;}

/* possible enpassant pawn capture */
if (flags & ENPASSANT)
   {mtl[oc]-=PAWNVAL;pawncol[oc][tcol]--;pawncnt[oc]--;
    if (t>f) x=t-8; else x=t+8;
    board[x]=EMPTY;brdcolor[x]=NONE;mvboard[x]++;}

/* possible castling move */
if (flags & CASTLE)
   {if (t>f) {x=f+3;y=x-2;} else {x=f-4;y=x+3;}
    board[x]=EMPTY;brdcolor[x]=NONE;mvboard[x]++;
    board[y]=ROOK; brdcolor[y]=c;
    castled[c]=TRUE;
   }

/* find new epsquare */
epsquare=-1;
if (piece==PAWN)
   {if (t-f==16) epsquare=f+8;
    if (f-t==16) epsquare=f-8;
   }

/* was a pawn involved in the move? */
if ( (piece==PAWN) || (captured==PAWN)) scorepawns();
}

void unmakemove(void)
/* this retracts a move and downdates the material balance, pawn count, king
   location and everything else that changes when you make a move */
{
register gametype *gameptr0;
gameptr0=gameptr;
game50=gameptr0->game50;epsquare=gameptr0->epsquare;
piece =gameptr0->piece; captured=gameptr0->captured;
f     =gameptr0->f;     t       =gameptr0->t;
flags =gameptr0->flags;
movenum--;gameptr=gameptr0-1;

c=brdcolor[t];fcol=getcol(f);tcol=getcol(t);
oc=chngcolor(c);

/* downdate a pawn promotion */
if (flags & PROMOTION)
   {x=flags & 7;
    mtl[c]=mtl[c]-pieceval[x]+PAWNVAL;
    pawncnt[c]++;pawncol[c][tcol]++;}

/* unmake a move */
board[f]=piece;brdcolor[f]=c;            /* move the piece */
board[t]=EMPTY;brdcolor[t]=NONE;         /* delete to position */
mvboard[f]--;                            /* decrement move count */

/* possible king location change */
if (piece==KING) kingloc[c]=f;
if (captured==KING) kingloc[oc]=t;

/* replace a captured piece */
if (captured)
   {board[t]=captured;brdcolor[t]=oc;mvboard[t]--;
    mtl[oc]+=pieceval[captured];}

/* replace a captured pawn */
if (captured==PAWN) {pawncnt[oc]++;pawncol[oc][tcol]++;}

/* possible pawn move */
if (piece==PAWN) {pawncol[c][fcol]++;pawncol[c][tcol]--;}

/* possible enpassant pawn capture */
if (flags & ENPASSANT)
   {mtl[oc]+=PAWNVAL;pawncol[oc][tcol]++;pawncnt[oc]++;
    if (t>f) x=t-8; else x=t+8;
    board[x]=PAWN;brdcolor[x]=oc;mvboard[x]--;}

/* possible castling move */
if (flags & CASTLE)
   {if (t>f) {x=f+3;y=x-2;} else {x=f-4;y=x+3;}
    board[y]=EMPTY;brdcolor[y]=NONE;
    board[x]=ROOK; brdcolor[x]=c;mvboard[x]--;
    castled[c]=FALSE;
   }

/* was a pawn involved in the move? */
if ( (piece==PAWN) || (captured==PAWN)) scorepawns();
}

void sort(movetype *_first,movetype *_last) /* 20.0% */
/* sort the moves in descending order the moves are from first+1 to last */
/* This is a fairly simple sort.  We regularly only have 30 or so moves,
   so more complicated ones (such as quicksort) are often slower.        */
{ static direct int s;
  register movetype *p2;
  static direct movetype *p3;
  static movetype temp;
  static movetype *first,*last;
  first=_first;
  last=_last;

first++;
while (first<last)
   {p2=first+1;s=(p2-1)->score;p3=first;
    while (p2<=last)
      {if (p2->score > s) {p3=p2;s=p2->score;}
       p2++;
      }
    if (first!=p3)
      {copymv(&temp,first);
       copymv(first,p3);
       copymv(p3,&temp);
      }
    first++;
   }
}
