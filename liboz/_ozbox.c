#include <oz.h>

void _ozbox(byte x,byte y,byte w,byte h)
{
    _ozvline(x,y,h,BLACK);
    _ozhline(x,y,w,BLACK);
    _ozvline(x+w-1,y,h,BLACK);
    _ozhline(x,y+h-1,w,BLACK);
}

