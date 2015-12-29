#include <stdio.h>
#include "cyrxlat.c"

main()
{
    int c;
    int i;
    while(-1!=(c=getchar()))
    {
        if(c&0x80) c=cp1251_to_koi8r[c&0x7f];
        for(i=0;i<NUM_KOI8R;i++)
            if((unsigned char)from_KOI8R[i][0]==(unsigned char)c)
            {
                c=from_KOI8R[i][1];
                break;
            }
        putchar(c);
    }
    return 0;
}

