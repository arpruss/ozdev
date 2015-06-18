#include <math.h>

#define RES 256
#define MULT 255
#define RES0 181  /* RES/sqrt(2) */


void main(void)
{
    int i,v;
    for(i=0;i<RES;i++)
    {
      v=sqrt(RES*(float)RES-i*i)/RES*MULT+0.5;
      printf("defb %d ; (%d)\n",(unsigned char)v,i);
    }
}

