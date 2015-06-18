#include <stdio.h>
#include <math.h>
#include <oz.h>

#define PI 3.1415926536

main()
{
    int i;
    char s[80];
    nonblank();
    for(i=0;i<240;i++)
    {
        float y;
        y=39-(sin((i/240.)*PI*2))*20+cos((i/240.)*20)*5;
        ozpoint(i,y,1);
    }
    sprintf(s,"Value = %f",cos(4.5));
    ozputs(10,10,s);
    for(i=0;i<360;i++)
    {
        float x,y;
        x=120+30*sin(i/360.*2*PI);
        y=40+30*cos(i/360.*2*PI);
        ozpoint(x,y,1);
    }
    for(i=0;i<240;i++)
    {
        float y=sqrt(i/240.);
        ozpoint(i,y*80,1);
    }
    ozsound(256);
    for(i=0;i<5000;i++);
    ozquiet();
    blank();
}

