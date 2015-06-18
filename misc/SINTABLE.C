#include <math.h>

main()
{
    long double x;
    int i;
    for(i=0;i<=90;i++)
    {
        printf("defw %d\n",(int)(sin(i*(M_PI/180.))*16384.));
    }
}
