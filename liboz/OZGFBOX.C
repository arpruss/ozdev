#include <oz.h>

void ozgreybox(int x,int y,int w,int h,byte shade)
{
    ozgreyline(x,y,x,y+h-1,shade);
    ozgreyline(x,y+h-1,x+w-1,y+h-1,shade);
    ozgreyline(x+w-1,y+h-1,x+w-1,y,shade);
    ozgreyline(x+w-1,y,x,y,shade);
}


void ozgreyfilledbox(int x,int y,int w,int h,byte shade)
{
    ozsetactivepage(1);
    ozfilledbox(x,y,w,h,shade>>1);
    ozsetactivepage(0);
    ozfilledbox(x,y,w,h,shade&1);
}
