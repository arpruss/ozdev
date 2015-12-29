#include <math.h>
#include <oz.h>
#include "graf.h"

/* simplifications of various internal routines */

void _ozhline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,len,1,color);
}

void _ozvline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,1,len,color);
}

int _ozpoint(byte x,byte y,byte color)
{
    return ozpoint(x,y,color);
}

double mypow(double _x, double _y)
{
    static double v;
    static int i;
    static double x,y;
    x=_x;
    y=_y;
    if(y==0.)
    {
        if(x==0.)
        {
            __fperr=1;
        }
        return 1.;
    }
    else if(y==0.5)
        return sqrt(x);
    v=_frndint(y);
    if(v!=y) return pow(x,y);
    if(v<10 && v>-10)
    {
        i=(int)v;
        if(i<0)
        {
            i=-i;
            x=1./x;
        }
        v=x;
        while(--i)
            v*=x;
        return v;
    }
    if(x>=0.) return pow(x,y);
    x=-x;
    if(y/2==_frndint(y/2))
        /* even */
        return pow(x,y);
    else
        return -pow(x,y);
}




