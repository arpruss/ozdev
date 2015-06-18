#include <oz.h>

void ozfilledbox(int x,int y,int w,int h,byte color)
{
    if(y>238) return;
    if(y<0)
    {
        w-=y;
        y=0;
    }
    if(x<0)
    {
        h-=x;
        x=0;
    }
    if(x+w>239) w=239-x;
    if(y+h>80) h=80-y;
    if(h<=0 || w<=0) return;
    _ozfilledbox(x , y , w , h , color);
}
