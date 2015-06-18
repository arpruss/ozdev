#include <oz.h>

int ozgreypoint(int x, int y, byte shade)
{
    ozsetactivepage(1);
    ozpoint(x,y,shade>>1);
    ozsetactivepage(0);
    return ozpoint(x,y,shade&1);
}


