#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 59 /* 60 is OK if bleeding isn't a problem */
#define HEIGHT 13

int ttyprint(byte c);
void ttyscroll(void);

extern char currow;
extern char curcol;

char *itoa(int u)
{
    static char buffer[10];
    register char *b;
    static byte neg;
    neg=0;
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
    currow++;
    if(currow>(HEIGHT-1))
    {
        currow--;
        ttyscroll();
    }
}

void setscroll(byte n,int nl)
{
    extern unsigned scrolltop, scrolllen, scrollbotline, scrollamount;
    scrolltop=0xa000;
    scrolllen=30*n*(nl-1);
    scrollbotline=0xa000+scrolllen;
    scrollamount=30*n;
}

void reset_term(void)
{
    ozcls();
    curcol=currow=0;
}

void crlf(void)
{
    curcol=0;
    currow++;
    if(currow>(HEIGHT-1))
    {
        currow--;
        ttyscroll();
    }
}

void termputch(byte c)
{
    switch(c)
    {
#if 0
        case 8:
            if(curcol>0) curcol--;
             else curcol=0;
            return;
        case 11:
        case 12:
#endif            
        case '\n': newline(); return;
        case '\r':
                curcol=0;
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
