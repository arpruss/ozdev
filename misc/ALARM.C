#include <oz.h>
#include <ctype.h>

#if 0
unsigned xtoa(char *s)
{
    unsigned x;
    sscanf(s,"%x",&x);
    return x;
}
#endif

main()
{
/*
    char c1[3];

    c1[0]=ozgetch();
    c1[1]=ozgetch();
    c1[2]=0;
    ozportout(0x19,0);
    ozportout(0x15,xtoa(c1));
    ozputs(10,10,c1);
*/
    while(!ozkeyhit())
    {
      ozsound(0); /* 8192); */
      ozdelay(2000);
      ozquiet();
      ozdelay(2000);
    }
}
