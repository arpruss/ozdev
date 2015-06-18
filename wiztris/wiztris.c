#include <stdlib.h>
#include <ctype.h>
#include <oz.h>

void ozandblock(int offset, char newbyte, byte num);
void ozorblock(int offset, char newbyte, byte num);
byte putrotated(int col,byte line,char *s);

#define HEADER 43


#define MAXHIGH 10



typedef struct
{
    long score;
    char initials[4];
} score_t;

score_t highscores[MAXHIGH];
byte numhigh=0;

byte memo[HEADER+2*(sizeof(numhigh)+sizeof(highscores[0])*MAXHIGH)+1]=
"MEMO\xFF\xFF\xD0\x0D\x00\x16\x00\x00\x00"
"20990101\x00zz:wiztris.data     \x00";
unsigned recnum=0;
unsigned memo_pos=0;
byte scores_changed=0;

#define BOX
#define WIDTH 232
#define HEIGHT 80
#define HEIGHT_DIV_2 40

#define NROTS 4
#define NSHAPES 7
#define shapetbl(i,j,k) _shape[(k)+4*(j)+16*(i)]
char _shape[112]; /* [4*4*7]; */
#define GWIDTH 10
#define GHEIGHT 22
#define GHEIGHT_1 21
char _grid[220];
#define grid(row,col) _grid[(col)+GWIDTH*(row)] /* row,col */

int maxheight;
int shownext;

long thescore=0;

int level;
int scores;
int delays[10]={ 45*280, 45*262, 45*233, 45*210, 45*175, 45*163, 45*140,
45*105, 45*81, 45*52};
int fulls;

unsigned mygetch(void)
{
    static unsigned k;
    k=ozgetch();
    if(k==KEY_CATEGORY)
    {
        ozsnap();
        ozsnap();
    }
    return k;
}

showlevel()
{
    char s[2];
    s[0]='0'+level;
    s[1]=0;
    putrotated(28,putrotated(28,0,"Level: "),s);
}

void addtoscore(int s)
{
    static char score[15];
    thescore+=s;
    sprintf(score,"%ld",thescore);
    putrotated(27,0,score);
}

init()
{
    static int i;
    thescore=0;
    fulls=0;
    maxheight=GHEIGHT;
    for(i=0;i<220;i++) _grid[i]=0;
    for(i=0;i<112;i++) _shape[i]=0;
#include "tristbl.c"
}

void dosquare(int i,int j,int c)  /* row,column,color */
{
    static int offset;
#ifndef BOX
#define SQ_LAST 8
#else
#define SQ_LAST last
    static byte last;
    if(j==9 && maxheight==GHEIGHT) last=7;
     else last=8;
#endif
    offset=i+j*240;
    if(!c)
/*
      for(k=1; k<last; k++) ozdisplayandbyte(offset+30*k,0x1);
*/
      ozandblock(offset+30,0x1,last-1);
    else
    {
/*
        for(k=1; k<8; k++) ozdisplayorbyte(offset+30*k,0xfe);
*/
        ozorblock(offset+30,0xfe,7);
    }
}

int dopiece(int row0,int col0,int n,int r,int c)
{
    static int row;
    static int col;
    static int colmask;
    static int st;
    for(row=0; row<4; row++)
    {
        st=shapetbl(n,row,r);
        if(st && row0-row>=0 && row0-row<maxheight)
        {
            colmask=1;
            for(col=0; col<4; col++)
            {
                if((colmask&st) && col+col0>=0 && col+col0<GWIDTH)
                    dosquare(row0-row,col+col0,c);
                colmask=colmask<<1;
            }
        }
    }
}

drawgrid()
{
    static int row;
    static int col;
    for(row=0; row<GHEIGHT; row++) for(col=0; col<GWIDTH; col++)
      dosquare(row,col,grid(row,col));
}

#ifdef __TURBOC__
#define upperkeyq lowerkeyq
#endif

int fitq(int row0,int col0,int piece,int rot)
{
    static int row,col,colmask,st;
    for(row=0; row<4; row++)
    {
        if(row0-row>=GHEIGHT) continue;
        st=shapetbl(piece,row,rot);
        if(st)
        {
            if(row0-row<0) return 0;
            colmask=1;
            if(st) for(col=0; col<4; col++)
            {
                if((colmask&st) &&
                (col+col0<0 || col+col0>=GWIDTH || grid(row0-row,col+col0)))
                    return 0;
                colmask=colmask<<1;
            }
        }
    }
    return 1;
}

int addtogrid(int row0,int col0,int piece,int rot)
{
    static int row,col,colmask,st;
    for(row=0; row<4; row++)
    {
        colmask=1;
        st=shapetbl(piece,row,rot);
	if(st) for(col=0; col<4; col++)
	{
	    if((colmask&st) && row0-row>=0 && row0-row<GHEIGHT &&
	    col+col0>=0 && col+col0<GWIDTH)
		grid(row0-row,col+col0)=1;
	    colmask=colmask<<1;
	}
    }
    return 1;
}

void killrow(int i)
{
    int i2,j;
    for(j=0;j<GWIDTH;j++) dosquare(i,j,0);
    for(i2=i;i2<GHEIGHT-1;i2++) for(j=0;j<GWIDTH;j++)
      grid(i2,j)=grid(i2+1,j);
    for(j=0;j<GWIDTH;j++) grid(GHEIGHT-1,j)=0;
    drawgrid();
    fulls++;
    if(fulls>=10 && level<9)
    {
      fulls-=10;
      level++;
      showlevel();
    }
}

void scanlines(int row)
{
    static int i,j;
    int full;
    i=row-4;
    if(i<0) i=0;
    for(;i<=row;i++)
    {
       full=1;
       for(j=0;j<GWIDTH;j++)
         if(!grid(i,j))
         {
            full=0;
            break;
         }
       if(full)
       {
         killrow(i);
         row--;
         i--;
       }
    }
}

int getlowestrow(int row,int piece,int rot)
{
    static int i;
    for(i=3;i>=0;i--)
      if(shapetbl(piece,i,rot)) return row-i;
}

int gethighestrow(int row,int piece,int rot)
{
    static int i;
    for(i=0;i<4;i++)
      if(shapetbl(piece,i,rot)) return row-i;
}


void doshownext(int piece,int rot,int c)
{
    maxheight=GHEIGHT+5;
    dopiece(maxheight-1,6,piece,rot,c);
    maxheight=GHEIGHT;
}

void help()
{
    ozcopypage(1,0);
    ozcls();
    putrotated(28,0,"MOVE:");
    putrotated(27,0,"< / > (either big or");
    putrotated(26,20,"small)");
    putrotated(25,0,"Z / lower-MENU");
    putrotated(24,0,"X / NEW");
    putrotated(22,0,"ROTATE:");
    putrotated(21,0,"v, Q, MAIN,");
    putrotated(20,0,"MY PROGRAMS,");
    putrotated(19,0,"upper-MENU");
    putrotated(17,0,"DROP:");
    putrotated(16,0,"., ENTER, MEMO,");
    putrotated(15,0,"TELEPHONE");
    putrotated(13,0,"Toggle next piece");
    putrotated(12,40,"display: N");
    putrotated(11,0,"Go up a level: ^");
    putrotated(10,0,"Exit: ESC");
    putrotated(9,0,"Pause: P");
    putrotated(8,0,"Image: category");
    putrotated(7,0,"Help: H");
    putrotated(5,0,"Press any key.");
    while(KEY_CATEGORY==mygetch());
    ozcopypage(0,1);
}

void drop()
{
    static unsigned c;
    static int piece;
    static int rot;
    static int row,col;
    static int i;
    static int rot2;
    static int col2;
    static int curdelay;
    static int droppedfrom;
    static int nextrot,nextpiece;
    static int curheight;
    static char buffer[25];
    rot=rand()%NROTS;
	piece=rand()%NSHAPES;
    col=3;
    while(ozkeylower(0xff));
    showlevel();
    if(numhigh)
    {
        sprintf(buffer,"%s %ld",highscores[0].initials,highscores[0].score);
        putrotated(26,0,buffer);
    }
    curheight=GHEIGHT_1-getlowestrow(0,piece,rot);
    while(fitq(curheight,col,piece,rot))
    {
        nextrot=rand()%NROTS;
        nextpiece=rand()%NSHAPES;
        if(shownext) doshownext(nextpiece,nextrot,1);
        curdelay=delays[level];
        droppedfrom=-1;
        for(row=curheight; row>=0; row--)
        {
           dopiece(row,col,piece,rot,1);
           for(i=0;i<curdelay;i++)
              if(ozkeyhit()) switch(c=ozgetch())
                 {
                   case 'n':
                            shownext=!shownext;
                            doshownext(nextpiece,nextrot,shownext);
                            break;
                   case KEY_UPPER_MENU:
                   case KEY_LEFT:
                   case KEY_MAIN:
                   case KEY_MYPROGRAMS:
                   case 'q':
                            rot2=(rot+1)%NROTS;
                            if(fitq(row,col,piece,rot2))
                            {
                               dopiece(row,col,piece,rot,0);
                               rot=rot2;
                               dopiece(row,col,piece,rot,1);
                            }
                            break;
                   case KEY_RIGHT:
                            if(level<9)
                              {
                                level++;
                                curdelay=delays[level];
                                showlevel();
                              }
                              break;
                   case KEY_LOWER_MENU:
                   case KEY_DOWN:
                   case KEY_NEW:
                   case KEY_PAGEDOWN:
                          col2=col+1;
                          if(fitq(row,col2,piece,rot))
                          {
                            dopiece(row,col,piece,rot,0);
                            col=col2;
                            dopiece(row,col,piece,rot,1);
                          }
                          break;
                   case 'h':
                      help();
                      break;
                   case 'z':
                   case 'x':
                   case KEY_PAGEUP:
                   case KEY_UP:
                      col2=col-1;
                      if(fitq(row,col2,piece,rot))
                      {
                        dopiece(row,col,piece,rot,0);
                        col=col2;
                        dopiece(row,col,piece,rot,1);
                      }
                      break;
                   case KEY_TELEPHONE:
                   case KEY_MEMO:
                   case KEY_UPPER_ENTER:
                   case KEY_LOWER_ENTER:
                   case '.':
                          curdelay=0;
                          droppedfrom=getlowestrow(row,piece,rot);
                      break;
                   case KEY_LOWER_ESC:
                   case KEY_UPPER_ESC:
                      return;
                   case KEY_BACKLIGHT:
                      oztogglelight();
                      break;
                   case KEY_POWER:
                      ozungetch(c);
                      exit(0);
                   case 'p':
                      ozcopypage(1,0);
                      ozcls();
                      putrotated(17,23,"Paused.");
                      putrotated(15,10,"Press any key");
                      putrotated(14,16,"to continue.");
                      ozgetch();
                      ozcopypage(0,1);
                      break;
                   case 'i':
                      ozsnap();
                      ozsnap();
                      ozsnap();
                      break;
                 }
           if(fitq(row-1,col,piece,rot))
             dopiece(row,col,piece,rot,0);
           else
           {
             if(gethighestrow(row,piece,rot)>=GHEIGHT)
             {
                return;
             }
             addtogrid(row,col,piece,rot);
             if(droppedfrom<0) droppedfrom=getlowestrow(row,piece,rot);
             if(!shownext)
               addtoscore(5+(2*level)+droppedfrom);
             else
               addtoscore(3+(3*level)/2+droppedfrom);
             scanlines(row);
             break;
           }
        }
        if(shownext) doshownext(nextpiece,nextrot,0);
        rot=nextrot;
        piece=nextpiece;
        col=3;
        curheight=GHEIGHT_1-getlowestrow(0,piece,rot);
    }
    dopiece(curheight,col,piece,rot,1);
}

void getinitials(byte x,byte y,char *initials)
{
    static byte i;
    static unsigned k;
    i=0;
    while(i<3)
    {
        switch(k=ozgetch())
        {
            case KEY_BACKSPACE:
                if(i)
                {
                    i--;
                    y-=putrotated(-1,0,initials+i);
                    putrotated(24,y,"   ");
                }
                break;
            case KEY_CATEGORY:
                ozsnap();
                ozsnap();
                break;
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
                exit(0);
            default:
                if(k<127 && (isalpha(k) || isdigit(k)))
                {
                    initials[i]=toupper(k);
                    initials[i+1]=0;
                    y=putrotated(24,y,initials+i);
                    i++;
                }
                break;
        }
    }
}



void add_high_score(void)
{
    static char initials[4];
    static char i2[2];
    static int i,j;
    static byte y;
    scores_changed=1;
    i2[1]=0;
    y=putrotated(24,0,"Initials: ");
    getinitials(24,y,initials);
    for(i=MAXHIGH-1;i>=0 && thescore>highscores[i].score;i--) ;
    i++;
    if(numhigh<MAXHIGH) numhigh++;
    for(j=numhigh-1;j>i;j--)
    {
        highscores[j].score=highscores[j-1].score;
        strcpy(highscores[j].initials,highscores[j-1].initials);
    }
    highscores[i].score=thescore;
    strcpy(highscores[i].initials,initials);
}

int ozload(int handle,byte *_s,int length)
{
    static int c1,c2;
    static int out;
    register byte *s=_s;
    out=0;
    while(length)
    {
        if(-1==(c1=ozreadfilebyte())) return out;
        if(c1>='a')
        {
            *s++='z'-c1;
        }
        else
        {
            if(-1==(c2=ozreadfilebyte())) return out;
            *s++ = ((c1-'A')<<4) | (c2-'a');
        }
        out++;
        length--;
    }
    return out;
}

void load_scores(void)
{
    static int handle;
    static int i;
    static unsigned a,b,loc;
    a=b=recnum=0;
    if(!(loc=ozfindmemo("zz:wiztris.data",&a,&b,&recnum)))
    {
        recnum=0;
        return;
    }
    ozopenfile(loc);
    for(i=0;i<HEADER;i++) ozreadfilebyte();
    if(ozload(handle,(byte*)&numhigh,1) < 1)
    {
        numhigh=0;
        return;
    }
    if(ozload(handle,(byte*)highscores,(int)numhigh*sizeof(highscores[0])) <
    numhigh*sizeof(highscores[0]))
    {
        numhigh=0;
        memset(highscores,MAXHIGH*sizeof(highscores[0]),0);
    }
    ozclosefile();
}

void putbyte(byte *s,byte data)
{
    s[0]=(data>>4) + 'A';
    s[1]=(data&0xF) + 'a';
}

void dump(byte *s,int length)
{
    while(length--)
    {
        if(*s<26)
            memo[memo_pos++]='z'-(*s++);
        else
        {
            putbyte(memo+memo_pos,*s++);
            memo_pos+=2;
        }
    }
}

void save_scores(void)
{
    if(!scores_changed) return;
    if(recnum) ozunlink(TYPENUM_MEMO,recnum);
    memo_pos=HEADER;
    dump((byte*)&numhigh,1);
    dump((byte*)highscores,(int)numhigh*sizeof(highscores[0]));
    memo[memo_pos++]=0;
    ozwritefile(memo,memo_pos,TYPENUM_MEMO);
}

void main()
{
    static unsigned k;
    static char buffer[15];
    static int c,i;
#ifndef __TURBOC__
    srand(ozsec()+ozmin()*60);
#else
    time_t t;
    extern int level;
    srand(time(&t));
#endif
    ozsetrepeatspeed(5);
    ozsetrepeatdelay(16);
    load_scores();
    ozclick(0);
    do
    {
      shownext=0;
      ozcls();
      putrotated(28,17,"Wiztris 1.96");
      putrotated(1,2,"Copyright (c) 2002");
      putrotated(0,5,"Alexander Pruss");
      putrotated(26,1,"USE AT YOUR OWN");
      putrotated(25,29,"RISK.");
      putrotated(23,3,"Press ESC to exit.");
      putrotated(22,6,"Press H for help.");
      putrotated(19,7,"Select level: 0-9");

      atexit(save_scores);

      if(numhigh)
      {
        putrotated(16,10,"HIGH SCORES:");
        for(i=0;i<numhigh;i++)
        {
           putrotated(14-i,0,highscores[i].initials);
           sprintf(buffer,"%ld",highscores[i].score);
           putrotated(14-i,79-putrotated(-1,0,buffer),buffer);
        }
      }
      while((k=ozgetch())=='h' || k==KEY_CATEGORY)
      {
         if(k=='h') help();
           else
           {
             ozsnap();
             ozsnap();
           }
      }
      if(k>='0' && k<='9') level=k-'0';
       else level=3;
      if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC)
        return;
      if(k==KEY_POWER)
      {
         ozungetch(k);
         return;
      }
      init();
      ozcls();
#ifdef BOX
      ozbox(0,0,177,80);
#endif
      addtoscore(0);
      drop();
      if(thescore<=highscores[MAXHIGH-1].score)
         putrotated(24,0,"Game over!");
      else
      {
         putrotated(25,0,"High score!");
         add_high_score();
      }
      putrotated(23,0,"Again? (Y/N)     ");
      while(ozkeyhit()) ozgetch();
      while((c=mygetch())!='n' && c!='y' && c!=KEY_MYPROGRAMS && c!=KEY_POWER
        && c!=KEY_LOWER_ESC && c!=KEY_UPPER_ESC);
   } while(c!='n' && c!=KEY_MYPROGRAMS && c!=KEY_POWER &&
   c!=KEY_LOWER_ESC && c!=KEY_UPPER_ESC);
   if(c==KEY_POWER) ozungetch(c);
}

