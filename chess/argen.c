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
static direct int othercolor,pos,pd;
static direct int nextmap,destpos,mappedpos,piece;
static direct int moves,temp;
   /* valid row for enpassant capture destination */
static direct int eprow;
static direct int *nxtoff;

static void storemv(int tpos)
/* store the move.  Give it a large bonus if it is a capture, otherwise
   use the history heuristic score.  Capture bonuses are based on the
   idea that it is best to do captures of the largest piece first,
   subsorted by the lowest capturing piece */
/* captures are given a 16k bonus so they will be higher than the
   bonus from the history heuristic, which will be 0 to 16k-1.  */
{ static int trow,p,c;
mvindex++;
mvindex->f=pos;
mvindex->t=tpos;
mvindex->flags=NO;
p=board[pos];c=board[tpos];

#ifdef HISTORY
if(c)
#endif
{mvindex->score=((c*8)-p)+6+16384;mvindex->flags|=CAPTURE;}
#ifdef HISTORY
else mvindex->score=readhistory(((pos << 6))+tpos) >> 2;
#endif


/* setting the flags is the last thing to do */
if (p==PAWN)
  {trow=getrow(tpos);
   if (tpos==epsquare) mvindex->flags|=ENPASSANT;
   if (((tpos>pos) && (trow==6)) || ((tpos<pos) && (trow==1)))
      {mvindex->score+=1000;mvindex->flags|=PROMTHRT;}
   if (trow==7 || trow==0)
      {mvindex->flags|=PROMOTION;
       mvindex->score+=2000;
       copymv(mvindex+1,mvindex);
       copymv(mvindex+2,mvindex);
       copymv(mvindex+3,mvindex);
   /* since I copy this, I MUST NOT do anything else to it because */
   /* the other copies wouldn't be updated                         */
       mvindex->flags+=QUEEN; mvindex++;
       mvindex->flags+=ROOK;  mvindex++;
       mvindex->flags+=BISHOP;mvindex++;
       mvindex->flags+=KNIGHT;
      }
  }

/* flag a castle move */
if ((p==KING) && ( ((pos-tpos)==2) || ((tpos-pos)==2)) )
   mvindex->flags|=CASTLE;
}

movetype *gencaps(movetype *buffer,int color)
/* gencaps generates only the captured moves.*/
/* Moves are stored from buffer+1 to xxxxx, inclusive. */
{
mvindex=buffer;
othercolor=chngcolor(color);moves=0;
for (pos=63;pos>=0;pos--)
  {if (brdcolor[pos]==color)
    {piece=board[pos];mappedpos=map[pos];nxtoff=pdir[piece];
     if (sweep[piece])
        {do
          {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
           while (destpos>=0)
              if (board[destpos]==EMPTY)
                   {nextmap+=*nxtoff;destpos=unmap[nextmap];moves++;}
              else if (brdcolor[destpos]==othercolor)
                      {storemv(destpos);destpos=-1;moves++;}
              else destpos=-1;
          } while (*(++nxtoff));
        }
     else if (piece==PAWN)  /* hardwire the directions in */
            {pd=mappedpos-11;eprow=2;
             if (color==WHITE) {pd=mappedpos+9;eprow=5;}
             /* hardwire the capture directions */
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     ((getrow(destpos)==eprow) && (destpos==epsquare)) )
                   {storemv(destpos);moves++;}
             pd+=2;
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     ((getrow(destpos)==eprow) && (destpos==epsquare)) )
                   {storemv(destpos);moves++;}
          /* generate pawn advancements onto other side of board */
             if ( (color==WHITE) && (board[pos+8]==EMPTY) )
               {moves++;if (pos>31) storemv(pos+8);}
             if ( (color==BLACK) && (board[pos-8]==EMPTY) )
               {moves++;if (pos<32) storemv(pos-8);}
            }
     else  /* king or knight */
        { do { if ((destpos=unmap[mappedpos+*nxtoff])>=0)
                 if ((temp=brdcolor[destpos])!=color)
                   {moves++;if (temp==othercolor) storemv(destpos);}
             } while (*(++nxtoff));
        }
    } /* if color */
  }; /* for */

/* Done.  All captures generated */
if (buffer!=mvindex) sort(buffer,mvindex);
else if (moves==0) mvindex=NULL;  /* flag no moves at all */
return(mvindex);
}

movetype *genmoves(movetype *buffer,int color)
/*genmoves generates all the moves */
{
mvindex=buffer;
othercolor=chngcolor(color);
for (pos=63;pos>=0;pos--)
  {if (brdcolor[pos]==color)
    {piece=board[pos];mappedpos=map[pos];nxtoff=pdir[piece];
     if (sweep[piece])
        {do
          {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
           while (destpos>=0)
              if (board[destpos]==EMPTY)
                 {storemv(destpos);nextmap+=*nxtoff;
                  destpos=unmap[nextmap];}
              else if (brdcolor[destpos]==othercolor)
                      {storemv(destpos);destpos=-1;}
              else destpos=-1;
          } while (*(++nxtoff));
        }
     else if (piece==PAWN)
            {if (color==WHITE)
              {pd=mappedpos+9; /* starting pawn capture direction */
               eprow=5;
               if (board[pos+8]==EMPTY)
                  {storemv(pos+8);
                   if ( (pos<16) && (board[pos+16]==EMPTY) )
                      storemv(pos+16);
                  }
               }
             else {pd=mappedpos-11; /* starting pawn capture direction */
                   eprow=2;
                   if (board[pos-8]==EMPTY)
                      {storemv(pos-8);
                       if ((pos>47) && (board[pos-16]==EMPTY))
                          storemv(pos-16);
                      }
                   }
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     ((getrow(destpos)==eprow) &&  (destpos==epsquare)) )
                   storemv(destpos);
             pd+=2;
             if ((destpos=unmap[pd])>=0)
                if ((brdcolor[destpos]==othercolor) ||
                     ((getrow(destpos)==eprow) && (destpos==epsquare)) )
                   storemv(destpos);
            }
     else  /* king or knight */
        {do {if ((destpos=unmap[mappedpos+*nxtoff])>=0)
               if (brdcolor[destpos]!=color) storemv(destpos);
            } while (*(++nxtoff));
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

movetype *genlegal(movetype *buffer,int color)
{movetype *tmpmv,*last,*legal;

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

bool sqatakd(int sq,int side)
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
  if ( ((piece=board[loc]) > PAWN) && (brdcolor[loc]==side) )
    {mappedpos=map[loc];
     nxtoff=pdir[piece];
     if (sweep[piece])
        {do
          {nextmap=mappedpos+*nxtoff;destpos=unmap[nextmap];
           while (destpos >= 0)
             {if (destpos==sq) return(TRUE);
              else if (board[destpos]==EMPTY)
                      {nextmap+=*nxtoff;destpos=unmap[nextmap];}
              else break;
             }
          } while (*(++nxtoff));
        }
     else {do {if (unmap[mappedpos+*nxtoff]==sq) return(TRUE);
              } while (*(++nxtoff));
          }
    }
return(FALSE);
}

