#include <oz.h>

main()
{
    unsigned size=ozgetauxsize();
    unsigned i,j;
    static char s[244];
    sprintf(s,"Size:  %d",size);
    ozputs(0,0,s);
    for(i=1;i<size;i++)
    {
        ozwriteauxbyte(i,i);
    }
    for(i=1;i<size;i++)
        if((byte)i!=(j=(byte)ozreadauxbyte(i)))
        {
            sprintf(s,"Error at %x: %x",i,j);
            ozputs(10,10,s);
            ozgetch();
        }
}
