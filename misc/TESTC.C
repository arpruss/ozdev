#include <oz.h>

main()
{
    char s[30];
    extern unsigned _oz1hz;
    _ozautoblanktime=16;
    ozgetchblank();
    while(!ozkeyhit() || 'q'!=ozgetch())
    {
        sprintf(s,"%d      ",_oz1hz);
        ozputs(10,10,s);
    }
}
