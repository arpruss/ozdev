#include <oz.h>
#include "same.h"

void putpiece(byte i,byte j,byte piece)
{
    static unsigned offset;
    offset=(j*PIECE_SIZE+y0)*30+i;
    _ozputsprite0(offset,PIECE_SIZE,pieces[piece&~GREY_MASK]);
    if(piece&GREY_MASK)
    {
        if(offset>30)
        {
            _ozputsprite0(offset-30,1,(byte*)"\xFF\xFF");
            _ozputsprite0(offset-29,1,(byte*)"\x1\x1");
        }
        _ozputsprite0(offset+1,PIECE_SIZE,left_line);
        _ozputsprite0(offset,PIECE_SIZE,selector);
    }
}

void hide_selection(void)
{
    static byte i;
    if(y0==0) i=PIECE_SIZE-1; else i=y0-1;
    for(;i<80;i+=PIECE_SIZE) _ozhline(0,i,PIECE_SIZE*MAXWIDTH+1,0);
    for(i=0;i<MAXWIDTH*PIECE_SIZE+1;i+=PIECE_SIZE)
        _ozvline(i,y0,80,0);
}

void showboard(void)
{
    static byte i,j;
    for(i=width;i;i--)
      for(j=height;j;j--)
        putpiece(i-1,j-1,board[i][j]);
}

void showcursor(byte i,byte j)
{
    _ozputsprite0((j*PIECE_SIZE+y0)*30+i,PIECE_SIZE,cursor);
}

void clearcursor(byte i,byte j)
{
    putpiece(i,j,board[i+1][j+1]);
}

unsigned cursor_and_key(byte i,byte j)
{
    static unsigned k;
    do
    {
        showcursor(i,j);
        k=ozkeydelay64hz(CURSOR_SPEED);
        clearcursor(i,j);
        if(k) return k;
    } while(!(k=ozkeydelay64hz(CURSOR_SPEED)));
    return k;
}

void showcount(unsigned selected)
{
    static char num[4];
    static byte x;
    _ozfilledbox(INFO_X,20,239-INFO_X,10,0);
    if(selected)
    {
        ozputs(INFO_X,20,"Bunch:");
        sprintf(num,"%u",selected);
        x=ozputs(0,-1,num);
        ozputs(239-x,20,num);
    }
}

void showrest(void)
{
    static byte x;
    static char num[4];
    _ozfilledbox(INFO_X,30,239-INFO_X,10,0);
    ozputs(INFO_X,30,"Rest:");
    sprintf(num,"%u",(unsigned int)rest);
    x=ozputs(0,-1,num);
    ozputs(239-x,30,num);
}

void showhigh(void)
{
    static char num[8];
    static byte x;
    static byte y;
    sprintf(num,"%u",highscores[current_size][current_number]);
    _ozfilledbox(INFO_X,50,239-INFO_X,30,0);
    ozputs(INFO_X,50,"High: ");
    x=ozputs(0,-1,num);
    if(x+(INFO_X+21)>239) y=60;
      else y=50;
    ozputs(239-x,y,num);
    ozputs(INFO_X,y+10,highscore_names[current_size][current_number]);
}

void showscore(void)
{
    static char num[8];
    static byte x;
    _ozfilledbox(INFO_X,0,239-INFO_X,20,0);
    sprintf(num,"%ld",score);
    ozputs(INFO_X,0,"Score:");
    x=ozputs(0,-1,num);
    ozputs(239-x,x+(INFO_X+27)>239?10:0,num);
}

void beep(void)
{
    if(current_quiet) return;
    ozinitsound();
    ozsound(30);
    ozdelay64hz(4);
    ozquiet();
}

void bounce(void)
{
    int x,y;
    char vx,vy;
    vx=1;
    vy=1;
    x=y=0;
#define WIDTH 82
#define DELTA1 13
#define DELTA2 5
#define HEIGHT (13*2+4+2)
    ozsavescreen();
    ozsetfont(FONT_PC_LARGE);
    ozsetdisplaypage(0);
    ozsetactivepage(1);
    do
    {
        ozrestorescreen();
        ozbox(x,y,WIDTH,HEIGHT);
        ozbox(x+1,y+1,WIDTH-2,HEIGHT-2);
        _ozfilledbox(x+2,y+2,WIDTH-4,HEIGHT-4,WHITE);
        ozputs(x+DELTA1,y+3,"Game Over!");
        ozputs(x+DELTA2,y+16,"Press any key");
        x+=vx;
        y+=vy;
        if(x+WIDTH>238)
        {
            vx=-1;
            x=238-WIDTH-1;
        }
        if(x<0)
        {
            vx=1;
            x=1;
        }
        if(y+HEIGHT>79)
        {
            vy=-1;
            y=79-HEIGHT-1;
        }
        if(y<0)
        {
            vy=1;
            y=1;
        }
        ozswapactivedisplay();
    } while(!ozkeydelay64hz(4));
    ozsetfont(FONT_PC_NORMAL);
    ozsetactivepage(0);
    ozrestorescreen();
    ozsetdisplaypage(0);
}

