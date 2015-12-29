#include <stdio.h>
#include "backgr.xbm"

main()
{
    int i,j;
    FILE *f=fopen("backdata.inc","w");
    for(i=0;i<2400;i+=8)
    {
        fprintf(f,"defb ");
        for(j=0;j<8;j++) fprintf(f,"%u%c",(unsigned int)(unsigned char)backgr_bits[i+j],j<7?',':'\n');
    }
    fclose(f);
    return 0;
}
