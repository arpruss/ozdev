/* This will crash and burn if the whole code+data doesn't fit in 8k */

#include <oz.h>
#include <string.h>
#include <ctype.h>

void
serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

/*
void read_line(int x, int y, char *s)
{
    int c;
    while((c=getch())!='\n')
    {
        *s++=c;
        x=ozputch_system(x,y,c);
    }
    *s=0;
    return;
}
*/

unsigned
htou(char *buf)
{
    unsigned val=0;
    while(isxdigit(*buf))
    {
        val*=16;
        if(isdigit(*buf)) val+=*buf-'0';
        else if(isupper(*buf)) val+=*buf-'A'+10;
        else val+=*buf-'a'+10;
        buf++;
    }
    return val;
}

char hexnibble(unsigned n)
{
    return n<10?n+'0':n+'A'-10;
}

char *utoh(unsigned n)
{
    static char hex[5];
    hex[4]=0;
    hex[3]=hexnibble(n&0xf);
    n>>=4;
    hex[2]=hexnibble(n&0xf);
    n>>=4;
    hex[1]=hexnibble(n&0xf);
    n>>=4;
    hex[0]=hexnibble(n&0xf);
    return hex;
}

char *btoh(byte n)
{
    static char hex[3];
    hex[2]=0;
    hex[1]=hexnibble(n&0xf);
    n>>=4;
    hex[0]=hexnibble(n&0xf);
    return hex;
}

void main()
{
    static char buf[120];
    register byte *b=(byte *)0xa000;
    unsigned page,i,j;
    int x;
    byte saved_lcr, saved_baud;
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD9600);
    ozputs_system(0,10,"Enter page address:");
    read_line(0,20,buf);
    page=htou(buf);
    x=ozputs_system(0,30,"Press a key to dump page ");
    ozputs_system(x,30,utoh(page));
    if(KEY_MYPROGRAMS==getch()) return;
    serputs("Page ");
    serputs(utoh(page));
    serputs("\r\n");
    ozportout(3,page&0xFF);
    ozportout(4,page>>8);
    for(i=0; i<8192; i+=16)
    {
        serputs(utoh(i));
        ozserialout(' ');
        for(j=0; j<16; j++)
        {
          serputs(btoh(b[i+j]));
          ozserialout(' ');
        }
        for(j=0; j<16; j++)
          if(b[i+j]>31 && b[i+j]<127) ozserialout(b[i+j]);
             else ozserialout('.');
        serputs("\r\n");
    }
    ozportout(3,7);
    ozportout(3,4);
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}

