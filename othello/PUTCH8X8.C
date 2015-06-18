#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 29 /* 30 is OK if bleeding isn't a problem */
#define HEIGHT 9
#define CHARHEIGHT 8

int ttyprint(byte c);
void ttyscroll(void);

extern char currow;
extern char curcol;
byte LNM=0;

void movecursor(int row, int col)
{
    xorcursor();
    currow=row;
    curcol=col;
    xorcursor();
}

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
    if(currow>HEIGHT)
    {
        currow--;
        ttyscroll();
    }
    xorcursor();
}

void reset_term(void)
{
    static byte i;
    ozcls();
    curcol=currow=0;
    LNM=0;
    xorcursor();
}

void crlf(void)
{
    xorcursor();
    curcol=0;
    currow++;
    if(currow>HEIGHT)
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
        case 7: beep();
            return;
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
        default:
            ttyprint(c);
            return;
    }
}

