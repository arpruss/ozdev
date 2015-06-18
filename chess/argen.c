/*
  This module is concerned with generating captures, all moves, and legal
  moves.  It also is concerned about square atack detection
*/

#define HEADER
#include "arvar.c"

/* This var is used only in the generation and storing of moves.  It
   holds the next_move_storage_location pointer.  It is easier than
   allways haveing to pass it as a parameter or update some other
   global pointer everytime.
*/
static movetype *mvindex;

/* static moves for the two move_generators().  They should be auto
   but it will be eaiser for asm translation if they are static */
static direct uchar othercolor;
static direct int pos,pd;
static direct int nextmap,destpos,mappedpos;
static direct uchar piece;
static direct uchar temp;
   /* valid row for enpassant capture destination */
static direct uchar eprow;
/* static direct int *nxtoff; */

void storemv(uchar tpos)
/* store the move.  Give it a large bonus if it is a capture, otherwise
   use the history heuristic score.  Capture bonuses are based on the
   idea that it is best to do captures of the largest piece first,
   subsorted by the lowest capturing piece */
/* captures are given a 16k bonus so they will be higher than the
   bonus from the history heuristic, which will be 0 to 16k-1.  */
{ static uchar p,c;
  static uchar trow;
  register movetype *mvindex0=mvindex;
mvindex0++;
mvindex0->f=pos;
mvindex0->t=tpos;
mvindex0->flags=NO;
p=board[pos];c=board[tpos];

#ifdef HISTORY
if(c)
#endif
{mvindex0->score=((c*8)-p)+6+16384;mvindex0->flags|=CAPTURE;}
#ifdef HISTORY
else mvindex0->score=readhistory(((pos << 3)<<3)+tpos) >> 2;
#endif


/* setting the flags is the last thing to do */
if (p==PAWN)
  {trow=getrow(tpos);
   if (tpos==epsquare) mvindex0->flags|=ENPASSANT;
   if (((tpos>(uchar)pos) && ((uchar)trow==6)) || ((tpos<(uchar)pos) && ((uchar)trow==1)))
      {mvindex0->score+=1000;mvindex0->flags|=PROMTHRT;}
   if (trow==7 || trow==0)
      {mvindex0->flags|=PROMOTION;
       mvindex0->score+=2000;
       copymv(mvindex0+1,mvindex0);
       copymv(mvindex0+2,mvindex0);
       copymv(mvindex0+3,mvindex0);
   /* since I copy this, I MUST NOT do anything else to it because */
   /* the other copies wouldn't be updated                         */
       mvindex0->flags+=QUEEN; mvindex0++;
       mvindex0->flags+=ROOK;  mvindex0++;
       mvindex0->flags+=BISHOP;mvindex0++;
       mvindex0->flags+=KNIGHT;
      }
  }

/* flag a castle move */
if ((p==KING) && ( (((uchar)pos-(uchar)tpos)==2) || (((uchar)tpos-(uchar)pos)==2)) )
   mvindex0->flags|=CASTLE;
mvindex=mvindex0;
}

movetype *gencaps(movetype *buffer,uchar color)      /* 4.4% */
/* gencaps generates only the captured moves.*/
/* Moves are stored from buffer+1 to xxxxx, inclusive. */
{
register int *nxtoff;
static direct uchar moves;
mvindex=buffer;
othercolor=chngcolor(color);moves=0;
for (pos=63;pos>=0;pos--)
  {if (brdcolor[pos]==color)
    {mappedpos=map[pos];
     switch((uchar)(piece=board[pos]))
     {
        case PAWN:
             if (color==WHITE) {pd=mappedpos+9;eprow=5;}
             else
             {
                pd=mappedpos-11;eprow=2;
             }
             /* hardwire the capture directions */
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     (((uchar)getrow(destpos)==(uchar)eprow) && ((uchar)destpos==(uchar)epsquare)) )
                   {storemv(destpos);moves=1;}
             pd+=2;
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     (((uchar)getrow(destpos)==(uchar)eprow) && ((uchar)destpos==(uchar)epsquare)) )
                   {storemv(destpos);moves=1;}
          /* generate pawn advancements onto other side of board */
             if ( (color==WHITE) && (board[pos+8]==EMPTY) )
               {moves=1;if ((uchar)pos>31) storemv(pos+8);}
             if ( (color==BLACK) && (board[pos-8]==EMPTY) )
               {moves=1;if ((uchar)pos<32) storemv(pos-8);}
            break;
        case BISHOP:
        case ROOK:
        case QUEEN:
          nxtoff=pdir[piece];
          do
          {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
            while (destpos>=0)
            {
              if (board[destpos]==EMPTY)
                   {nextmap+=*nxtoff;destpos=unmap[nextmap];moves=1;}
              else if (brdcolor[destpos]==othercolor)
                      {storemv(destpos);moves=1;break;}
              else break;
            }
          } while (*(++nxtoff));
          break;
        default:  /* king or knight */
         nxtoff=pdir[piece];
         do { if ((destpos=unmap[mappedpos+*nxtoff])>=0)
                 if ((temp=brdcolor[destpos])!=color)
                   {moves=1;if (temp==othercolor) storemv(destpos);}
             } while (*(++nxtoff));
       }
    } /* if color */
  }; /* for */

/* Done.  All captures generated */
if (buffer!=mvindex) sort(buffer,mvindex);
else if (moves==0) mvindex=NULL;  /* flag no moves at all */
return(mvindex);
}

movetype *genmoves(movetype *buffer,uchar color)
/*genmoves generates all the moves */
{
register int *nxtoff;
mvindex=buffer;
othercolor=chngcolor(color);
for (pos=63;pos>=0;pos--)
  {if (brdcolor[pos]==color)
    {piece=board[pos];mappedpos=map[pos];
     switch((uchar)piece)
      {
        case PAWN:
            {if (color==WHITE)
              {pd=mappedpos+9; /* starting pawn capture direction */
               eprow=5;
               if (board[pos+8]==EMPTY)
                  {storemv(pos+8);
                   if ( ((uchar)pos<16) && (board[pos+16]==EMPTY) )
                      storemv(pos+16);
                  }
               }
             else {pd=mappedpos-11; /* starting pawn capture direction */
                   eprow=2;
                   if (board[pos-8]==EMPTY)
                      {storemv(pos-8);
                       if (((uchar)pos>47) && (board[pos-16]==EMPTY))
                          storemv(pos-16);
                      }
                   }
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     (((uchar)getrow(destpos)==(uchar)eprow) &&  ((uchar)destpos==(uchar)epsquare)) )
                   storemv(destpos);
             pd+=2;
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     (((uchar)getrow(destpos)==(uchar)eprow) && ((uchar)destpos==(uchar)epsquare)) )
                   storemv(destpos);
            }
            break;
        case BISHOP:
        case ROOK:
        case QUEEN:
            nxtoff=pdir[piece];
            {do
              {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
               while (destpos>=0)
               {
                  if (board[destpos]==EMPTY)
                     {storemv(destpos);nextmap+=*nxtoff;
                      destpos=unmap[nextmap];}
                  else if (brdcolor[destpos]==othercolor)
                          {storemv(destpos);break;}
                  else break;
               }
              } while (*(++nxtoff));
            }
            break;
         default: /* king or knight */
            nxtoff=pdir[piece];
            {do {if ((destpos=unmap[mappedpos+*nxtoff])>=0)
                   if (brdcolor[destpos]!=color) storemv(destpos);
                } while (*(++nxtoff));
            }
      }
    } /* if color */
  }; /* for */


/* check for castling move */
pos=4;if (color==BLACK) pos=60;
if ((!castled[color]) && (mvboard[pos]==0) &&
       (kingloc[color]==pos) && (!sqatakd(pos,othercolor)))
/* king side castling */
   {if ((board[pos+3]==ROOK) && (mvboard[pos+3]==0))
      if ((board[pos+1]==EMPTY) && (board[pos+2]==EMPTY))
         if ((!sqatakd(pos+1,othercolor)) && (!sqatakd(pos+2,othercolor)) )
             storemv(pos+2);
/* queen side castling */
    if ((board[pos-4]==ROOK) && (mvboard[pos-4]==0))
      if ((board[pos-1]==EMPTY) && (board[pos-2]==EMPTY) &&
                  (board[pos-3]==EMPTY))
         if ((!sqatakd(pos-1,othercolor)) && (!sqatakd(pos-2,othercolor))
                                  && (!sqatakd(pos-3,othercolor)))
             storemv(pos-2);
    }

/* Done.  All moves generated */
sort(buffer,mvindex);
return(mvindex);
}

movetype *genlegal(movetype *buffer,uchar color)
{register movetype *tmpmv;
 static movetype *last,*legal;

findpieces();
legal=last=genmoves(buffer,color);

tmpmv=buffer;
while (tmpmv < last)
   {tmpmv++;
    makemove(tmpmv);
    if (sqatakd(kingloc[color],chngcolor(color)))
       {tmpmv->score=-1;legal--;}
    unmakemove();
   }
sort(buffer,last);
return(legal);
}

bool sqatakd(uchar sq,uchar side)
/*
  see if any piece with color 'side' ataks sq.  first check for pawns
  or king, then try other pieces. array dcode is used to check for
  knight attacks or r,b,q co-linearity.
*/
{
static direct int destpos;
static uchar nextmap;
register int *nxtoff;
static char delta,mappedpos;

mappedpos = map[sq]; /* map square onto 12x12 board */

/* does a pawn attack here? */
if (side==BLACK) nextmap=mappedpos+9;
   else nextmap=mappedpos-11;
if ((destpos=unmap[nextmap]) >=0)
   if ((board[destpos]==PAWN) && (brdcolor[destpos]==side)) return TRUE;
if ((destpos=unmap[nextmap+2]) >=0)
   if ((board[destpos]==PAWN) && (brdcolor[destpos]==side)) return TRUE;

/* does the king attack here? */
if (distance(sq,kingloc[side])==1) return TRUE;

nxtoff=pdir[ROOK];
delta=pdir[ROOK][0];
do
{
    nextmap=(uchar)(char)(mappedpos+delta);
    while((destpos=unmap[nextmap])>=0)
    {
        switch(board[destpos])
        {
            case 0:
                break;
            case ROOK:
            case QUEEN:
                if(brdcolor[destpos]==side) return TRUE;
            default:
                goto OUTER_CONTINUE2;
        }
        nextmap+=delta;
    }
OUTER_CONTINUE2:;
} while(delta=*(char*)(++nxtoff));

nxtoff=pdir[BISHOP];
delta=pdir[BISHOP][0];
do
{
    nextmap=(uchar)(char)(mappedpos+delta);
    while((destpos=unmap[nextmap])>=0)
    {
        switch(board[destpos])
        {
            case 0:
                break;
            case BISHOP:
            case QUEEN:
                if(brdcolor[destpos]==side) return TRUE;
            default:
                goto OUTER_CONTINUE1;
        }
        nextmap+=delta;
    }
OUTER_CONTINUE1:;
} while(delta=*(char*)(++nxtoff));

nxtoff=pdir[KNIGHT];
delta=pdir[KNIGHT][0];
do
{
    destpos=unmap[(uchar)(char)(mappedpos+delta)];
    if(destpos>=0 && board[destpos]==KNIGHT && brdcolor[destpos]==side)
        return TRUE;
} while(delta=*(char*)(++nxtoff));
return FALSE;
}

#ifdef TEST
bool sqatakd0(uchar sq,uchar side)
/*
  see if any piece with color 'side' ataks sq.  first check for pawns
  or king, then try other pieces. array dcode is used to check for
  knight attacks or r,b,q co-linearity.
*/
{
static direct int nextmap,destpos,mappedpos,loc,piece;
register int *nxtoff;

mappedpos = map[sq]; /* map square onto 12x12 board */

/* does a pawn attack here? */
nextmap=mappedpos-11;if (side==BLACK) nextmap=mappedpos+9;
if ((destpos=unmap[nextmap]) >=0)
   if ((board[destpos]==PAWN) && (brdcolor[destpos]==side)) return(TRUE);
if ((destpos=unmap[nextmap+2]) >=0)
   if ((board[destpos]==PAWN) && (brdcolor[destpos]==side)) return(TRUE);

/* does the king attack here? */
if (distance(sq,kingloc[side])==1) return(TRUE);

for (loc=63;loc>=0;loc--)
  if ( ((uchar)(piece=board[loc]) > PAWN) && (brdcolor[loc]==side) )
    {mappedpos=map[loc];
     nxtoff=pdir[piece];
     if (sweep[piece])
        {do
          {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
           while (destpos >= 0)
             {if ((uchar)destpos==(uchar)sq) return(TRUE);
              else if (board[destpos]==EMPTY)
                      {nextmap+=*nxtoff;destpos=unmap[nextmap];}
              else break;
             }
          } while (*(++nxtoff));
        }
     else {do {if ((uchar)unmap[mappedpos+*nxtoff]==(uchar)sq) return(TRUE);
              } while (*(++nxtoff));
          }
    }
return(FALSE);
}

bool sqatakd(uchar sq,uchar side)
{
    extern uchar currow;
    static uchar x1,x2;
    x1=sqatakd0(sq,side);
    x2=sqatakd1(sq,side);
    if(!!x1==!!x2) return !!x1;
    printboard();
    currow=0;
    atright();
    Bell();
    printf("%d,%d,%d,%d",x1,x2,sq,side);
    ozgetch();
    return x1;
}
#endif
