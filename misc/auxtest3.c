#include <oz.h>

#define LEN 555

main()
{
    unsigned size=ozgetauxsize();
    unsigned i,j;
    static char s[222];
    static byte data[LEN];
    sprintf(s,"Size:  %d",size);
    ozputs(0,0,s);
    for(i=1;i<size-LEN;i+=LEN)
    {
        for(j=0;j<LEN;j++)
            data[j]=i+j;
        sprintf(s,"W:%d",i);
        ozputs(10,10,s);
        ozwriteauxmem(i,data,LEN);

    }
    for(i=1;i<size-LEN;i+=LEN)
    {
        ozreadauxmem(i,data,LEN);
        for(j=0;j<LEN;j++)
           if(data[j]!=(byte)(i+j))
        {
            sprintf(s,"Error at %x+%x:%x",i,j,data[j]);
            ozputs(10,10,s);
            ozgetch();
        }
    }
}
