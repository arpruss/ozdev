#include <oz.h>

char hexnibble(unsigned n)
{
    return n<10?n+'0':n+'A'-10;
}

char *btoh(byte n)
{
    static char hex[3];
    hex[2]=0;
    hex[1]=hexnibble(n&0xf);
    n>>=4;
    hex[0]=hexnibble(n&0xf);
    return hex;
}

main()
{
    int x;
    x=ozputs(0,10,"Model ID #: ");
    ozputs(x,10,btoh(ozgetmodel()));
    ozputs(0,20,"Press any key to continue...");
    ozgetch();
}
