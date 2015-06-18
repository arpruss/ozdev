#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 59 /* 60 is OK if bleeding isn't a problem */
#define HEIGHT 13
#define CHARHEIGHT 6

int ttyprint(byte c);
void ttyscroll(void);

extern char currow;
extern char curcol;
char region_top=0;
char region_bottom=HEIGHT-1;
byte LNM=0;

char *itoa(int u)
{
    static char buffer[10];
    register char *b;
    char neg=0;
    b=buffer+9;
    *b=0;
    if(u<0)
    {
        u=-u;
        neg=1;
    }

    while(u || b==buffer+9)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    if(neg)
    {
        b--;
        *b='-';
    }
    return b;
}

static void newline(void)
{
    xorcursor();
    currow++;
    if(currow>region_bottom)
    {
        currow--;
        ttyscroll();
    }
    xorcursor();
}

static void scrolling_region(char row1, char row2)
{
    extern unsigned scrolltop, scrolllen, scrollbotline;
    row1--;
    row2--;
    if(row1<0) row1=0;
    if(row1>=HEIGHT) row1=HEIGHT-1;
    if(row2<row1) row2=row1;
    if(row2>=HEIGHT) row2=HEIGHT-1;
    region_top=row1;
    region_bottom=row2;
    scrolltop=(row1)*(30*6)+0xa000;
    scrolllen=(row2-row1)*(30*6);
    scrollbotline=0xa000+(row2)*(30*6);
}

void reset_term(void)
{
    static byte i;
    ozcls();
    curcol=currow=0;
    scrolling_region(1,HEIGHT);
    LNM=0;
    xorcursor();
}

void crlf(void)
{
    xorcursor();
    curcol=0;
    currow++;
    if(currow>region_bottom)
    {
        currow--;
        ttyscroll();
    }
    xorcursor();
}

void termputch(byte c)
{
    switch(c)
    {
        case 8: xorcursor();
            if(curcol>0) curcol--;
             else curcol=0;
            xorcursor();
            return;
        case 11:
        case 12:
        case '\n': newline(); return;
        case '\r':
            if(LNM) crlf();
            else
            {
                xorcursor(); curcol=0; xorcursor();
            }
            return;
        case 14: /* ignore G1 charset */
        case 15: /* ignore G0 charset */
        case 17: /* ignore XON */
        case 19: /* ignore XOFF */
        case 127: /* fill character, ignore */
          return;
        default:
            ttyprint(c);
            return;
    }
}

void termputs(char *s)
{
    while(*s) termputch(*s++);
}
