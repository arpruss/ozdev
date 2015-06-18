#include <oz.h>

void ozgreycircle(int x,int y,byte r,byte color)
{
    ozsetactivepage(1);
    ozcircle(x,y,r,color>>1);
    ozsetactivepage(0);
    ozcircle(x,y,r,color&1);
}

void ozgreyfilledcircle(int x,int y,byte r,byte color)
{
    ozsetactivepage(1);
    ozcircle(x,y,r,(color>>1) | FILL);
    ozsetactivepage(0);
    ozcircle(x,y,r,(color&1) | FILL);
}

