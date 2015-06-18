#include <math.h>
#include <oz.h>

main()
{
    float theta;
    for(theta=0; theta<M_PI*2; theta+=(M_PI*2)/2000)
        ozpoint(40+sin(theta)*39,40+cos(theta)*39,BLACK);
}

