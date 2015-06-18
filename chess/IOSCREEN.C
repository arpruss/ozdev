/*
  This handles the display of the board and other information.
  No effort was made to make a 'fancy' display.
*/


#define HEADER
#include "arvar.c"

_ozpoint(uchar x,uchar y,uchar c);

extern uchar currow,curcol;

static void printpiece(uchar row,uchar col)
/* print the piece or square.  Reverse colors for black piece or square */
{
 static int pos,piece;
 static char c1;
 static bool parity;
 static uchar x,y;
 static uchar i;

pos=row*8+col;piece=board[pos];
parity=!odd(row+col);
if(!font)
{
    if(piece)
    {
        if(brdcolor[pos]==BLACK && parity)
        {
            reversevideo=0xFF;
            c1=piece+11;
        }
        else if(brdcolor[pos]==BLACK)
        {
            c1=piece+17;
        }
        else if(parity)
        {
            c1=piece+17;
            reversevideo=0xFF;
        }
        else
        {
            c1=piece+11;
            reversevideo=0;
        }
    }
   else
    {
        c1=' ';
        if(parity) reversevideo=0xFF;
    }
}
else
{
    if (piece)
     {
        c1=piece+25;
        if(brdcolor[pos]==WHITE) reversevideo=0xFF;
     }
    else
      {
        if(parity) c1=25; else c1=' ';
      }
}
if(smallboard)
  outchar(c1);
else
{
  x=SCREENX(SCREENCOL(col));
  y=SCREENY(SCREENROW(row));
  putbitmap(1+x,1+y,8,font8x8+8*c1);
  if(parity)
  {
    for(i=0;i<10;i++)
    {
        _ozpoint(x+i,y,1);
        _ozpoint(x,y+i,1);
        _ozpoint(x+9,y+i,1);
        _ozpoint(x+i,y+9,1);
    }
  }
}
reversevideo=0;
}

void atright(void)
{
    curcol=13;
}

void printboard(void)
/*prints the board on the screen. also displays various stats*/
{
static int row,col;

clrscr();
currow=0;
atright();
printf(archess_version);
findpieces();

atright();
printf("To move: %s\n",(tomove==WHITE)?"WHITE":"BLACK");
atright();
printf("Move #: %d\n",movenum+shifted_gamehist);
/* atright();
printf("Est score:%d\n",estscore); */

atright();
printf("Material: %d\n",mtl[tomove]-mtl[chngcolor(tomove)]);

atright();
if (tcontrol) printf("TM: %d\n",responsetime);
else           printf("Level: %d\n",playlevel);
atright();
printf("Time used: %d\n",ElapClock);

curcol=currow=0;

if(smallboard)
{
if (reverse)
  {printf("\2HGFEDCBA\n");
   for (row=0;row<=7;row++)
     {printf("%d ",row+1);
      for (col=7;col>=0;col--) printpiece(row,col);
      printf(" %d\n",row+1);
     }
   printf("\2HGFEDCBA");
  }
else {
    printf("\2ABCDEFGH\n");
    for (row=7;row>=0;row--)
       {printf("%d ",row+1);
        for (col=0;col<=7;col++) printpiece(row,col);
        printf(" %d\n",row+1);
       };
    printf("\2ABCDEFGH");
   }
}
else
{
    for(row=0;row<8;row++) for(col=0;col<8;col++) printpiece(row,col);
}
}

