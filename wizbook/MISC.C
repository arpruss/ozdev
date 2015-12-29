#include <oz.h>
#include "wizbook.h"

void _ozhline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,len,1,color);
}

void _ozvline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,1,len,color);
}

void clear240(byte y0,byte h)
{
    _ozfilledbox(0,y0,240,h,0);
}

void ozcls(void)
{
    clear240(0,80);
}

void errread(void)
{
    err(error_read);
}
