#include <oz.h>

unsigned freq[12][2]=
{ { 941,1336 },
  { 697,1209 },
  { 697,1336 },
  { 697,1447 },
  { 770,1209 },
  { 770,1336 },
  { 770,1447 },
  { 852,1209 },
  { 852,1336 },
  { 852,1447 },
  { 941,1209 },
  { 941,1447 } };

void ozplay(unsigned freq)
{
    ozsound((16384/freq)-2);
    return;
}

void mydelay(unsigned i)
{
    while(i) i--;
}

/* 0.0000147 per i if optimized */


void dialdigit(byte i)
{
    static int j;
    static unsigned int del,fr;
    del=68027./freq[i][0]/2;
    fr=16384./freq[i][1]-2;
    for(j=0;j<600;j++)
    {
        ozsound(fr);
        mydelay(del);
        ozquiet();
        mydelay(del);
    }
    ozquiet();
    mydelay(10000);
    if(ozkeyhit()) exit(0);
}

void dialstring(char *s)
{
    int x=0;
    while(*s)
    {
        x=ozputch(x,0,*s);
        dialdigit(*s-'0');
        s++;
    }
}

void ozquiet(void);

main()
{
    static int i;
    atexit(ozquiet);
/*
    for(i=0;i<1000;i++)
      mydelay(10000);
*/

#asm
di
#endasm
    ozinitsound();
    dialstring("2977488");
#asm
ei
#endasm
}


