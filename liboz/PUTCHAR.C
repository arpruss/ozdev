#include "oz.h"

byte _ozcurx=0,_ozcury=10;
extern byte _ozfontniceheight;

static void _oznewline(void)
{
    _ozcurx=0;
    if(_ozcury+(_ozfontniceheight<<1)>81)
        ozscrolllinesclear(_ozfontniceheight);
    else
        _ozcury+=_ozfontniceheight;
}

int putchar(char c)
{
    if(c=='\n')
    {
        _oznewline();
        return 0;
    }
    if(ozputch(0,-1,c)+_ozcurx>238)
        _oznewline();
    _ozcurx=ozputch(_ozcurx,_ozcury,c);
    return 0;
}

void ozgotoxy(int x,int y)
{
    if(x>238) _ozcurx=238; else if(x<0) _ozcury=0; else _ozcurx=x;
    if(y>79) _ozcury=79; else if(y<0) _ozcury=0; else _ozcury=y;
}

