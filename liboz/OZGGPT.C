#include <oz.h>

int ozgreygetpoint(int x,int y)
{
    static int c1;
    ozsetactivepage(1);
    c1=ozgetpoint(x,y);
    if(c1<0) return 0;
    ozsetactivepage(0);
    return (c1<<1) | ozgetpoint(x,y);
}
