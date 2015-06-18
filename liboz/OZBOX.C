#include <oz.h>

void ozbox(int x,int y,int w,int h)
{
    ozline(x,y,x,y+h-1,BLACK);
    ozline(x,y+h-1,x+w-1,y+h-1,BLACK);
    ozline(x+w-1,y+h-1,x+w-1,y,BLACK);
    ozline(x+w-1,y,x,y,BLACK);
}

