#include <stdlib.h>
#include <oz.h>

#define NUM_CIRCLES 5

#define MAX_RADIUS 20
#define MIN_RADIUS 5

int x[NUM_CIRCLES];
int y[NUM_CIRCLES];
int r[NUM_CIRCLES];
int vx[NUM_CIRCLES];
int vy[NUM_CIRCLES];

main()
{
    int i;
    srand(ozsec()+60*ozmin());
    for(i=0;i<NUM_CIRCLES;i++)
    {
        r[i]=rand()%(MAX_RADIUS-MIN_RADIUS)+MIN_RADIUS;
        x[i]=rand()%(238-(1+2*r[i]))+r[i];
        y[i]=rand()%(79-(1+2*r[i]))+r[i];
        switch(rand()%4)
        {
            case 0: vx[i]=-1; vy[i]=-1; break;
            case 1: vx[i]=1; vy[i]=1; break;
            case 2: vx[i]=1; vy[i]=-1; break;
            case 3: vx[i]=-1; vy[i]=1; break;
        }
    }
    ozcls();
    ozsetactivepage(1);
    while(!ozkeyhit())
    {
        ozcls();
        for(i=0;i<NUM_CIRCLES;i++)
        {
            if(!(rand()%200))
            {
                switch(rand()%4)
                {
                    case 0: vx[i]=-1; vy[i]=-1; break;
                    case 1: vx[i]=1; vy[i]=1; break;
                    case 2: vx[i]=1; vy[i]=-1; break;
                    case 3: vx[i]=-1; vy[i]=1; break;
                }
            }
            x[i]+=vx[i];
            if(x[i]+r[i]>237 || x[i]-r[i]<1)
                vx[i]=-vx[i];
            y[i]+=vy[i];
            if(y[i]+r[i]>78 || y[i]-r[i]<1)
                vy[i]=-vy[i];
            ozcircle(x[i],y[i],r[i],4|BLACK);
        }
        ozswapactivedisplay();
    }
}
