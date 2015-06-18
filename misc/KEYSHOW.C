#include <oz.h>

main()
{
    unsigned k;
    char buf[20];
    ozsetpowerkeyhandling(0);
    while(KEY_UPPER_ESC!=(k=ozgetch()))
    {
        sprintf(buf,"%04x    ",k);
        ozputs(10,10,buf);
    }
}
