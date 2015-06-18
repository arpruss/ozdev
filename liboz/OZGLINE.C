#include <oz.h>

void ozgreyline(int _x1,int _y1,int _x2,int _y2,byte color)
{
    ozsetactivepage(1);
    ozline(_x1,_y1,_x2,_y2,color>>1);
    ozsetactivepage(0);
    ozline(_x1,_y1,_x2,_y2,color&1);
}

