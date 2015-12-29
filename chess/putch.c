#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 30
#define HEIGHT 10

int ttyprint(byte c);
void ttyscroll(void);

extern char currow;
extern char curcol;

void crlf(void)
{
    curcol=0;
    currow++;
    if(currow>=HEIGHT) currow=HEIGHT-1;
}

void termputch(byte c)
{
    switch(c)
    {
        case 8:
            if(curcol>0) curcol--;
             else curcol=0;
            return;
        case '\n':
            crlf();
            return;
        default:
            ttyprint(c);
            return;
    }
}

