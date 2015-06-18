#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 30
#define HEIGHT 10

void ttyprint(byte c);

extern byte currow;
extern byte curcol;

void crlf(void)
{
    curcol=0;
    currow++;
    if(currow>=HEIGHT) currow=HEIGHT-1; /* no scrolling! */
}

void termputch(byte c)
{
    switch(c)
    {
        case 8:
            if(curcol) curcol--;
            return;
        case '\n':
            crlf();
            return;
        default:
            if(c<8)
            {
                while(c--) ttyprint(' ');
            }
            else if(c & 0x80)
            {
                ttyprint(c&(byte)0x7F);
                ttyprint(' ');
            }
            else ttyprint(c);
            return;
    }
}

