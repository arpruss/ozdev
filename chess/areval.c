/*
  This module is concerned with evaluating the position
*/

#define HEADER
#include "arvar.c"

/* NOTE: The evaluator and stuff were heavily influenced by
         GNU Chess 3.0's evaluator    I have not implemented all
         of it, and probably will not do so.  However, I believe
         I have left enough 'hooks' to add in extra features here
*/

/* various weights, bonuses, and penalties */
/* Some of these could be defines, but there are a few that are adjusted
   in updateweights().  */

#define PADJ  6     /* pawn adjacent to another pawn */
#define PDBLD -14     /* doubled pawn */
#define PDERNK2    -10     /* central pawn (D & E) on 2nd rank */
#define PDERNK2B   -15     /*  & blocked by own piece */
#define PISO       -24     /* isolated pawn */
#define PSUP         5     /* pawn support same color piece */
#define PWNTHRT     40     /* pawn threatening to promote! */
#define MBLTY        4     /* multplier for bishop/rook mobility */
#define RHOPN        5     /* rook on half open column (1 pawn) */
#define ROPN        12     /* rook on open column (0 pawns) */
#define K0PADJF    -60     /* King has no pawn on adjacent files */
#define K1PADJF    -28     /* King has 1 pawn  on adjacent files */
#define K2PADJF     -8     /* King has 2 pawns on adjacent files */


static direct int KCASTLD;       /* bonus for early castle */
static direct int KMOVD;         /* penalty for moving before castling */
static direct int KSFTY;         /* multiplier for edge[] */
/* static direct int K0PADJF;       /* King has no pawn on adjacent files */
/* static direct int K1PADJF;       /* King has 1 pawn  on adjacent files */
/* static direct int K2PADJF;       /* King has 2 pawns on adjacent files */
static direct int KPSHLD;        /* each pawn near the king */
/* static direct int MBLTY;         /* multplier for bishop/rook mobility */
/* static direct int PADJ;          /* pawn adjacent to another pawn */
static direct int PBONUS[3];     /* pawn advantage bonus for each side */
/* static direct int PDBLD;         /* doubled pawn */
/* static direct int PDERNK2;       /* central pawn (D & E) on 2nd rank */
/* static direct int PDERNK2B;      /*  & blocked by own piece */
/* static direct int PISO;          /* isolated pawn */
/* static direct int PSUP;          /* pawn support same color piece */
/* static direct int PWNTHRT;       /* pawn threatening to promote! */
/* static direct int RHOPN;         /* rook on half open column (1 pawn) */
/* static direct int ROPN;          /* rook on open column (0 pawns) */

/* edge bonus value for king evaluation */
static tiny edge[64]=
   {0,1,2,3,3,2,1,0,
    1,2,3,4,4,3,2,1,
    2,3,4,5,5,4,3,2,
    3,4,5,6,6,5,4,3,
    3,4,5,6,6,5,4,3,
    2,3,4,5,5,4,3,2,
    1,2,3,4,4,3,2,1,
    0,1,2,3,3,2,1,0};

/* knight location value for evaluation */
static tiny pknight[64]=
   { 0, 6,11,14,14,11, 6, 0,
     6,12,22,25,25,22,12, 6,
    11,20,30,36,36,30,20,11,
    14,25,36,44,44,36,25,14,
    14,25,36,44,44,36,25,14,
    11,20,30,36,36,30,20,11,
     6,12,22,25,25,22,12, 6,
     0, 6,11,14,14,11, 6, 0};

/* bishop location value for evaluation */
static tiny pbishop[64]=
   {14,14,14,14,14,14,14,14,
    14,18,18,18,18,18,18,14,
    14,18,22,22,22,22,18,14,
    14,18,22,22,22,22,18,14,
    14,18,22,22,22,22,18,14,
    14,18,22,22,22,22,18,14,
    14,18,18,18,18,18,18,14,
    14,14,14,14,14,14,14,14};

/* vars used throughout the move evaluation */
static int pwnscr[3];     /* the pawn positional scores */
static int piecemtl[3];   /* piece material (no pawns or kings) */
static int stage,stage2;  /* the stage of the game */

static int genmob(int pos)
/* this finds the pawn based mobility.  (the mobility it would have if there
   was only pawns on the board) */
{
 static direct int other,cnt;
 static direct int mpos,unpos,m;
 register int *nxtoff;

cnt=0;other=chngcolor(brdcolor[pos]);mpos=map[pos];
nxtoff=pdir[board[pos]];
do
  {m=mpos+*nxtoff;unpos=unmap[m];
   while (unpos>=0)
      if (board[unpos]!=PAWN) { m+=*nxtoff;unpos=unmap[m];cnt++; }
      else {if (brdcolor[unpos]==other) cnt++;
            unpos=-1;}
  } while (*(++nxtoff));
return(cnt);
}

static int taxicab(int x,int y)
/* the distance the queen would have to travel to reach the other king if
   it could only move straight lines (no diagonals).  In other words, the
   absolute distance of the rows, plus the absolute distance of the columns */
{
return( abs(getcol(x)-getcol(y))+abs(getrow(x)-getrow(y)) );
}

/* misc vars used in scorepos & scorepawns */
static int pscore[3];
static direct int sqr,piece,color,score;
static direct int leftcol,rightcol,row,col,numpawns;
static direct int okingloc,pawnshield;
static direct int unmaped,mapedsqr,*nxtoff;

void scorepawns(void)
/* The pawns are only evaluated when one of them move.  This saves
   a little time in most evaluations.
*/
/*
  pawn: 1: pawn on column E or D on home row          +PDERNK2
        2: 1 above with next square blocked           +PDERNK2B
        3: more than 1 pawn same color on column      +PDBLD
        4: 0 pawns same color on either side          +PISO
        5: about to promote (others home row)         +PWNTHRT
        6: pawn next to another pawn                  +PADJ
*/
{
pawnevals++;
pwnscr[WHITE]=pwnscr[BLACK]=0;

for (sqr=56;sqr >=7 ;--sqr)
  if (board[sqr]==PAWN)
    {col=getcol(sqr);row=getrow(sqr);
     if (brdcolor[sqr]==WHITE)   /* WHITE PAWN */
        {if ((sqr==11) || (sqr==12))
            {pwnscr[WHITE]+=PDERNK2;
             if (board[sqr+8]!=EMPTY) pwnscr[WHITE]+=PDERNK2B;}
         if (pawncol[WHITE][col]>1) pwnscr[WHITE]+=PDBLD;
         if (col==0) leftcol=col+1;  else leftcol=col-1;
         if (col==7) rightcol=col-1; else rightcol=col+1;
         if (pawncol[WHITE][leftcol]+pawncol[WHITE][rightcol]==0)
            pwnscr[WHITE]+=PISO;
         if (row==6) pwnscr[WHITE]+=PWNTHRT;
         if (col<7)
            if ((board[sqr+1]==PAWN) && (brdcolor[sqr+1]==WHITE))
               pwnscr[WHITE]+=PADJ;
         if (col>0)
            if ((board[sqr-1]==PAWN) && (brdcolor[sqr-1]==WHITE))
               pwnscr[WHITE]+=PADJ;
        }   /* WHITE pawn */
     else { /* BLACK pawn */
         if ((sqr==51) || (sqr==52))
            {pwnscr[BLACK]+=PDERNK2;
             if (board[sqr-8]!=EMPTY) pwnscr[BLACK]+=PDERNK2B;}
         if (pawncol[BLACK][col]>1) pwnscr[BLACK]+=PDBLD;
         if (col==0) leftcol=col+1;  else leftcol=col-1;
         if (col==7) rightcol=col-1; else rightcol=col+1;
         if (pawncol[BLACK][leftcol]+pawncol[BLACK][rightcol]==0)
             pwnscr[BLACK]+=PISO;
         if (row==1) pwnscr[BLACK]+=PWNTHRT;
         if (col<7)
             if ((board[sqr+1]==PAWN) && (brdcolor[sqr+1]==BLACK))
               pwnscr[BLACK]+=PADJ;
         if (col>0)
             if ((board[sqr-1]==PAWN) && (brdcolor[sqr-1]==BLACK))
               pwnscr[BLACK]+=PADJ;
        } /* else BLACK pawn */
    }   /* if pawn */
}

static int scorepos(int side)
{
/*the heuristics are:
  pawn  : 1: the pre-computed pawn score              +pwnscr[]
          2: pawn supporting your piece               +PSUP
  knight: 1: the value of the pknight array           +PKNIGHT[]
  bishop: 1: the value of the pbishop array           +PBISHOP[]
          2: the pawn based mobility for this sqr     +genmob()
  rook:   1: the pawn based mobility for this sqr     +genmob()
          2: if 0 pawns on column                     +ROPN
          3: if 1 pawns on column                     +RHOPN
  queen:  1: taxicab distance to enemy king
          2: if close to enemy king                   +12
  king: if piecemtl[color] <=1300 then endgame (1300 is arbitrary)
           1: edge[]
        else middlegame
           1: KSFTY*edge[]
           2: K0PADJF, K1PADJF, K2PADJF depending on how many columns next
              to him have pawns of same color.
           3: each same color pawn around king        +KPSHLD
           4: if king has castled                     +KCASTLD
           5: if king has moved before castling       +KMOVD
*/

pscore[WHITE]=pscore[BLACK]=pscore[NONE]=0;

sqr=63;
do
  {piece=board[sqr];color=brdcolor[sqr];score=0;
   col=getcol(sqr);row=getrow(sqr);
   switch (piece) {
      case (PAWN)  :{if (color==WHITE)   /* WHITE PAWN */
                       {if ( (row!=7) && (col!=0) )
                           if (brdcolor[sqr+7]==WHITE) score+=PSUP;
                        if ( (row!=7) && (col!=7) )
                           if (brdcolor[sqr+9]==WHITE) score+=PSUP;
                        break;
                        }
                     /* BLACK PAWN */
                     if ( (row!=0) && (col!=7) )
                        if (brdcolor[sqr-7]==BLACK) score+=PSUP;
                     if ( (row!=0) && (col!=0) )
                        if (brdcolor[sqr-9]==BLACK) score+=PSUP;
                     break;}
      case (KNIGHT):{ score=pknight[sqr] >> 1;break;}
      case (BISHOP):{ score=(pbishop[sqr] >> 1)+genmob(sqr)*MBLTY;break;}
      case (ROOK)  :{ score=genmob(sqr)*MBLTY;
                      numpawns=pawncol[WHITE][col]+pawncol[BLACK][col];
                      if (numpawns==0) score+=ROPN;
                      else if (numpawns==1) score+=RHOPN;
                      break;}
      case (QUEEN) :{ okingloc=kingloc[chngcolor(color)];
                      score=(14-taxicab(sqr,okingloc))*2;
                      if (distance(sqr,okingloc) < 3) score+=12;
                      break;}
      case (KING)  :{if (piecemtl[color]>1300) { /* opening or middle game */
                       score+=KSFTY*edge[sqr];
                       if (castled[color]) score+=KCASTLD;
                       else if (mvboard[color]) score+=KMOVD;
                       pawnshield=0;
                       if (col==0) pawnshield++;
                          else if (pawncol[color][col-1]) pawnshield++;
                       if (pawncol[color][col]) pawnshield++;
                       if (col==7) pawnshield++;
                          else if (pawncol[color][col+1]) pawnshield++;
                       if (pawnshield==0) score+=K0PADJF;
                       else if (pawnshield==1) score+=K1PADJF;
                       else if (pawnshield==2) score+=K2PADJF;
                       mapedsqr=map[sqr];
                       nxtoff=pdir[KING];
                       do {if ((unmaped=unmap[mapedsqr+*nxtoff])>=0)
                             if (board[unmaped]==PAWN) score+=KPSHLD;
                            } while (*(++nxtoff));
                       break;}
                     /* end game */
                     score+=edge[sqr];
                     break;}
     } /*end switch*/
   pscore[color]+=score;
  } while ( (--sqr) >= 0);

score=pscore[side]-pscore[chngcolor(side)]+
      pwnscr[side]-pwnscr[chngcolor(side)];
return(score);
}

#define WINDOW PAWNVAL*2

int evalu8(movetype *move,int prevpscr,int side,
           int lowbound,int hibound,int depth,bool *incheck,int qsearch)
/* prevpscr=the previous positional score (no material) */
{ static int score,np,other;
  static bool evflag;

other=chngcolor(side);
score=(-prevpscr)+mtl[side]-mtl[other];

evflag=(depth<2) || ( depth <= level ) ||
       ((!qsearch) && (score>lowbound-WINDOW) && (score<hibound+WINDOW)) ||
       ((qsearch)  && (score>lowbound-50) && (score<hibound+50));

/* check whether anybody is in check */
if (sqatakd(kingloc[other],side)) return(BIGNUM+1);
*incheck=sqatakd(kingloc[side],other);

if (evflag)
  {score=mtl[side]-mtl[other];
   updateweights(depth);
   /* add in the pawn ratio (better for you to have pawns than him) */
   score=score+PBONUS[side]-PBONUS[other];

   score+=scorepos(side);
   move->flags|=FULLSRCH;
   evals++;
  }

return(score);
}

/* Just so we know when mtl balance has changed and to updateweights */
static oldwhite,oldblack=-BIGNUM;

void updateweights(int depth)
{ static int temp;
  static int np;

if ( (depth<2) || (oldwhite!=mtl[WHITE]) || (oldblack!=mtl[BLACK]) )
  {
   oldwhite=mtl[WHITE];oldblack=mtl[BLACK];

   np=pawncnt[WHITE]+pawncnt[BLACK];
   if (np)
     {PBONUS[BLACK]=pawncnt[BLACK]*100/np;
      PBONUS[WHITE]=pawncnt[WHITE]*100/np;}
   else PBONUS[BLACK]=PBONUS[WHITE]=0;

   piecemtl[WHITE]=mtl[WHITE]-KINGVAL-pawncnt[WHITE]*PAWNVAL;
   piecemtl[BLACK]=mtl[BLACK]-KINGVAL-pawncnt[BLACK]*PAWNVAL;
   piecemtl[NONE]=0;

   temp=piecemtl[WHITE]+piecemtl[BLACK];
   if (temp > 6600)      stage  = 0;
   else if (temp < 1400) stage  = 10;
   else                  stage  = (6600-temp)/520;

   if (temp>3600)        stage2 = 0;
   else if (temp < 1400) stage2 = 10;
   else                  stage2 = (3600-temp)/220;

#if 0
   PADJ=         6;     /* pawn adjacent to another pawn */
   PDBLD=      -14;     /* doubled pawn */
   PDERNK2=    -10;     /* central pawn (D & E) on 2nd rank */
   PDERNK2B=   -15;     /*  & blocked by own piece */
   PISO=       -24;     /* isolated pawn */
   PSUP=         5;     /* pawn support same color piece */
   PWNTHRT=     40;     /* pawn threatening to promote! */

   MBLTY=        4;     /* multplier for bishop/rook mobility */

   RHOPN=        5;     /* rook on half open column (1 pawn) */
   ROPN=        12;     /* rook on open column (0 pawns) */
#endif

   KCASTLD=  10-stage;  /* bonus for early castle */
   KMOVD=-40/(stage+1); /* penalty for moving before castling */

#if 0
   K0PADJF=    -60;     /* King has no pawn on adjacent files */
   K1PADJF=    -28;     /* King has 1 pawn  on adjacent files */
   K2PADJF=     -8;     /* King has 2 pawns on adjacent files */
#endif
   KPSHLD =  10-stage;  /* each pawn near the king */
   if (stage < 8) KSFTY=16-2*stage; else KSFTY=0;

  scorepawns();
  }
}

