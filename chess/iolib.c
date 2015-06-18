/*
  These routines are just misc routines used by IO.C
*/

#include <ozinput.h>
#include <ozscreen.h>

#define HEADER
#include "arvar.c"
/* #define BENCHMARK /* allow timing commands */

_ozpoint(uchar x,uchar y,uchar c);


/* The boards at the start of a game */
static tiny sboard[64]=
    {ROOK  ,KNIGHT,BISHOP,QUEEN ,KING  ,BISHOP,KNIGHT,ROOK  ,
     PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,
     EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,
     EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,
     EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,
     EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,EMPTY ,
     PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,PAWN  ,
     ROOK  ,KNIGHT,BISHOP,QUEEN ,KING  ,BISHOP,KNIGHT,ROOK};

static tiny scolor[64]=
    {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,
     WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,
     NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,
     NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,
     NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,
     NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,NONE ,
     BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
     BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};

void clearboard(void)
/*this clears the board*/
{/* register int x; */
memset(board,EMPTY,64);
memset(brdcolor,NONE,64);
memset(mvboard,0,sizeof mvboard);
/*
for (x=63;x>=0;x--)
   {board[x]=EMPTY;brdcolor[x]=NONE;mvboard[x]=0;}
*/
newgame();
}

void newboard(void)
/* this sets up a new board */
{/* register int x; */
memset(mvboard,0,sizeof mvboard);
memcpy(board,sboard,64);
memcpy(brdcolor,scolor,64);
/*
for (x=63;x>=0;x--)
   {board[x]=sboard[x];brdcolor[x]=scolor[x];mvboard[x]=0;}
*/
newgame();
}

void newgame(void)
{static int x;
game50=movenum=shifted_gamehist=0;
gameptr=gamehist;
epsquare=-1;
findpieces();
mate=draw=FALSE;
/* pvsmsg[0]=*/ msg[0]=status[0]=0;
timeout=FALSE;extratime=0;resetclock();
for (x=63;x>=0;x--)
   if ((board[x]==sboard[x]) && (brdcolor[x]==scolor[x])) mvboard[x]=0;
   else mvboard[x]=10;
castled[WHITE]=castled[BLACK]=FALSE;
}

void initgame(void)
{
newboard();responsetime=30;extratime=0;playlevel=3;tcontrol=TRUE;
msg[0]=/* pvsmsg[0]=*/ status[0]=0;
epsquare=-1;
estscore=0;
/* breakpress=*/ mate=bothsides=reverse=/* pvsflag=*/FALSE;
qsearch=TRUE;
computer=BLACK;human=WHITE;tomove=human;
resetclock();elaptime();
}

int cmdis(char *str)
/* this is just an easier way to check if str is the cmdline */
{
    static char *s;
    register char *p=cmdline;
    if(p[1]>='0' && p[1]<='9') return 0;
    s=str;
    while(*s)
        if(*s++!=*p++) return 0;
    return 1;
}

static uchar pos_row=8,pos_col=0;
static uchar sel_row=8,sel_col=0;
static char *ptr;
static char *iptr;
static uchar startrow,startcol;
static uchar curpos,posx,posy;

static void gotostring(void)
{
    currow=startrow;
    curcol=startcol+(ptr-iptr);
}

static void bs(void)
{
    termputs("\8 \8");
}

static void clearstring(void)
{
    gotostring();
    while((--ptr)>=iptr) bs();
    ptr=iptr;
}

static bool initmove(void)
{
    if(!movepieces) return 0;
    if(pos_row==8)
    {
        pos_row=3;
        pos_col=3;
        sel_row=8;
        clearstring();
        return 0;
    }
    return 1;
}

static uchar bigcursor[][2]=
{
    { 0,0 },
    { 0,1 },
    { 1,0 },

    { 8,0 },
    { 9,0 },
    { 9,1 },

    { 0,8 },
    { 0,9 },
    { 1,9 },

    { 9,9 },
    { 9,8 },
    { 8,9 },

    { 2,0 },
    { 9,2 },
    { 8,1 },
    { 0,7 },
    { 2,9 },
    { 1,8 },
    { 7,9 },
    { 9,7 },
    { 0,2 },
    { 1,1 },
    { 7,0 },
    { 8,8 } };

#define NUMPAIRS (sizeof bigcursor / 2)

static void docursor(uchar pos_col,uchar pos_row,uchar n)
{
    static uchar x,y,i;
      x=SCREENX(pos_col);
      y=SCREENY(pos_row);
      for(i=0;i<n;i++) _ozpoint(x+bigcursor[i][0],y+bigcursor[i][1],2);
}

static void mark(void)
{
    static uchar to;
    static uchar col;
    static uchar row;
    if(smallboard || !movenum || doenter) return;
    to=gamehist[movenum].t;
    col=getcol(to);
    row=getrow(to);
    docursor(SCREENCOL(col),SCREENROW(row),12);
}

static void xorcursor()
{
    if(pos_row==8 || pos_row==sel_row && pos_col==sel_col) return;
    if(smallboard)
    {
      currow=pos_row+1;
      curcol=pos_col+2;
      doxor=XORHL;
      termputch(24);
      doxor=NOP;
    }
    else
    {
      docursor(pos_col,pos_row,NUMPAIRS);
    }
}

static void unselect(void)
{
   pos_col=sel_col;
   pos_row=sel_row;
   sel_row=8;
   xorcursor();
}

static void setxy(void)
{
   if(reverse)
   {
     posx=7-pos_col;
     posy=pos_row;
   }
   else
   {
     posx=pos_col;
     posy=7-pos_row;
   }
   curpos=8*posy+posx;
}

static void emitcur(void)
{
   gotostring();
   setxy();
   termputch(*ptr++=posx+'A');
   termputch(*ptr++=posy+'1');
}

static uchar mypiece(void)
{
    setxy();
    if(brdcolor[curpos]!=tomove) return 0;
    return board[curpos];
}

#define LEN 19
void getline(void)
{
    static unsigned c;
    static uchar piece;
    static uchar marked;
    ptr=iptr=cmdline;
    startcol=curcol;
    startrow=currow;
    sel_row=8;
    mark(); marked=1;
    if(movepieces && !smallboard)
            initmove();
    while(1)
    {
        xorcursor();
        c=ozgetch();
        if(marked)
        {
            mark();
            marked=0;
        }
        xorcursor();
        switch(c)
        {
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            case KEY_UP:
                if(initmove() && pos_row) pos_row--;
                break;
            case KEY_DOWN:
                if(initmove() && pos_row<7) pos_row++;
                break;
            case KEY_RIGHT:
                if(initmove() && pos_col<7) pos_col++;
                break;
            case KEY_LEFT:
                if(initmove() && pos_col) pos_col--;
                break;
            case KEY_MYPROGRAMS:
                strcpy(iptr,"QUIT");
                return;
            case KEY_UPPER_ESC:
            case KEY_LOWER_ESC:
                if(movepieces && sel_row!=8)
                {
                    unselect();
                }
                clearstring();
                break;
            case KEY_BACKSPACE:
                gotostring();
                if(pos_row!=8)
                {
                    if(sel_row!=8)
                    {
                        unselect();
                        clearstring();
                    }
                }
                else
                if(ptr>iptr)
                {
                    --ptr;
                    bs();
                }
                break;
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
            case '\r':
                gotostring();
                if(movepieces && pos_row!=8)
                {
                    setxy();
                    if(sel_row==8 || sel_row!=8 && brdcolor[curpos]==tomove)
                    {
                        if(sel_row<8)
                        {
                            static uchar temp_row,temp_col;
                            temp_row=pos_row;
                            temp_col=pos_col;
                            unselect();
                            pos_row=temp_row;
                            pos_col=temp_col;
                        }
                        if(doenter || (piece=mypiece()))
                        {
                            xorcursor();
                            sel_row=pos_row;
                            sel_col=pos_col;
                            ptr=iptr;
                            emitcur();
                            break;
                        }
                        else
                        {
                            Bell();
                            break;
                        }
                    }
                    else
                    {
                            xorcursor();
                            emitcur();
                            if(piece==PAWN && (tomove==WHITE && posy==7
                                || tomove==BLACK && posy==0))
                            {
                                currow=8;
                                atright();
                                termputs("Promot\xE5""to\xBF""QRBN");
                                c=ozgetch();
                                switch(c)
                                {
                                    case 'q':
                                    case 'r':
                                    case 'b':
                                    case 'n':
                                        gotostring();
                                        termputch(*ptr++=c&(0xFF-0x20));
                                        break;
                                    default:
                                        unselect();
                                        goto LEAVESWITCH;
                                }
                            }
                            *ptr=0;
                            return;
                    }
                }
                *ptr=0;
                return;
            default:
                if(c>127 || c<32) break;
                gotostring();
                if(movepieces && pos_row!=8)
                {
                    if(sel_row==8)
                    {
                        unselect();
                    }
                    pos_row=8;
                }
                if(c>='a') c&=0xff-0x20;
                if(ptr+1<iptr+LEN && curcol<28)
                {
                    *ptr++=c;
                    termputch(c);
                }
                break;
        }
        LEAVESWITCH:;
    }
}

void readcmd(void)
/* Read a string from the keyboard.  Maximum of 20 characters long.
   Also, convert all lowercase to UPPER case so we can compare commands */
{
 static int len;
 getline();
 len=strlen(cmdline);
 if (len)
   if (cmdline[len-1]=='\n') {cmdline[len-1]='\0';len--;}
}


bool verifymove(void)
/* verify that the humans move is valid */
{
 static int f,t,prom /* ,x,start */;
 static char c,r;
 static bool valid;
 register movetype *mv;
 static movetype *tmv;
 static movetype *last;
 movetype buffer[MAXPLYMVS];

findpieces();
valid=FALSE;prom=0;
strcpy(msg,"INVALI\xC4""CMD");
if(mate||draw) return 0;
f=kingloc[tomove];
if (cmdis("O-O-O")) t=f-2;
else if (cmdis("O-O")) t=f+2;
else {if (strlen(cmdline)<=3) return(0);
      c=(cmdline[0])-'A';r=(cmdline[1])-'1';f=r*8+c;
      c=(cmdline[2])-'A';r=(cmdline[3])-'1';t=r*8+c;
      if(c<0 || r<0 || c>7 || r>7) return 0;
      switch(cmdline[4])
      {
        case 'Q': prom=QUEEN; break;
        case 'R': prom=ROOK; break;
        case 'B': prom=BISHOP; break;
        case 'N': prom=KNIGHT; break;
        default:  prom=0; break;
      }
     }
strcpy(msg,"ILLEGA\xCC""MOVE");
last=genlegal(buffer,tomove);

mv=NULL;tmv=buffer;
while (tmv < last)
  {tmv++;
   if ((f==tmv->f) && (t==tmv->t) && ((tmv->flags & 7)==prom) )
      mv=tmv;
  }

if (mv)
  {
   processmove(mv);tomove=chngcolor(tomove);valid=TRUE;
   strcpy(msg,"Yo\xF5""did:");
   mvenglish(mv,msg+7);
  }
else checkstatus(tomove);
return(valid);
}

static byte savedlight;

void fast(void)
{
    if(doblank)
    {
        ozfast();
        savedlight=ozgetlight();
        ozsetlight(0);
    }
}

void slow(void)
{
    if(doblank)
    {
        ozslow();
        ozsetlight(savedlight);
    }
}

#ifdef BENCHMARK

void gentime(void)
{int x;
 movetype buffer[MAXPLYMVS];
         fast();
resetclock();
for (x=0;x < 2000; x++)  genmoves(buffer,tomove);
elaptime();

itoa(2000/ElapClock,msg);
strcat(msg," genmoves/sec");
slow();
}

void captime(void)
{int x;
 movetype buffer[MAXPLYMVS];

fast();
resetclock();
for (x=0;x < 2000; x++)  gencaps(buffer,tomove);
elaptime();

itoa(2000/ElapClock,msg);
strcat(msg," gencaps/sec");
slow();
}

void evaltime(void)
{int x;
 movetype fakemv;bool check;
/*
*/
fast();
updateweights(0);
level=10;
  /* set search level so full eval, but no updateweights */
resetclock();
for (x=0;x < 4000; x++)
  {
   estscore=evalu8(&fakemv,0,tomove,-BIGNUM,BIGNUM,5,&check,FALSE);
  }
elaptime();

itoa(4000/ElapClock,msg);
strcat(msg," evalu8/sec");
slow();
}
#endif
