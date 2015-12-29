#include <stdio.h>
#include "cyrxlat.c"

main()
{
    int c;
    int i;
    while(-1!=(c=getchar()))
    {
        for(i=0;i<NUM_KOI8R;i++)
            if(from_KOI8R[i][1]==c)
            {
                c=from_KOI8R[i][0];
                break;
            }
        putchar(c);
    }
    return 0;
}

