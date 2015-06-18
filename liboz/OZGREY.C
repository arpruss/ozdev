#include <oz.h>

int ozsetgreyscale(byte grey)
{
    extern byte _ozgreyscale;
    extern unsigned _ozgreycount;
    _ozgreyscale=!!grey;
    if(grey)
    {
        ozportout(7,ozportin(7)&(0xFF-64));
        _ozgreycount=110;
    }
    else
    {
       ozportout(7,ozportin(7)|64);
       ozsetactivepage(0);
    }
    return 0;
}

byte ozgetgreyscale(void)
{
    extern byte _ozgreyscale;
    return _ozgreyscale;
}
