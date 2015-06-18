#include <math.h>
#include <oz.h>

#define RES     320
#define DIVIDER 127
char xtable[RES/8], ytable[RES/8];

ozcircle(byte x0,byte y0,int r,byte c)
{
    byte i;
    int x,y;
    for(i=0;i<RES/8;i++)
    {
      x=xtable[i]*r/DIVIDER;
      y=ytable[i]*r/DIVIDER;
      ozpoint(x0+x,y0+y,c);
      ozpoint(x0-x,y0+y,c);
      ozpoint(x0+x,y0-y,c);
      ozpoint(x0-x,y0-y,c);
      ozpoint(x0+y,y0+x,c);
      ozpoint(x0-y,y0+x,c);
      ozpoint(x0+y,y0-x,c);
      ozpoint(x0-y,y0-x,c);
    }
}

main()
{
    int i;
    for(i=1; i<RES/8; i++)
    {
        int x=DIVIDER*cos((float)i*(2*M_PI/RES));
        xtable[i]=x;
        x=DIVIDER*sin((float)i*(2*M_PI/RES));
        ytable[i]=x;
    }
    xtable[0]=DIVIDER;
    ytable[0]=0;
    ozcircle(40,40,39,1);
    ozgetch();
}
