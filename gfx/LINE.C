#include <stdlib.h>
#include <oz.h>

#define OUTERNUM 20
#define NUM 100

byte x[NUM];
byte y[NUM];

main()
{
    int i;
    int j;
    for(j=0;j<OUTERNUM;j++)
    {
        for(i=0;i<NUM;i++)
        {
          x[i]=rand()%239;
          y[i]=rand()%80;
          if(i) ozline(x[i-1],y[i-1],x[i],y[i],BLACK);
        }
        for(i=1;i<NUM;i++)
          ozline(x[i-1],y[i-1],x[i],y[i],WHITE);
    }
    ozgetch();
}
