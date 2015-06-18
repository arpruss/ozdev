/* This will crash and burn if the whole code+data doesn't fit in 8k */

#include <oz.h>
#include <string.h>
#include <ctype.h>

void
serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

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

void dump(unsigned page)
{
    static unsigned i;
    static byte j;
    register byte *b=(byte *)0xa000;
    ozputs(0,30,utoh(page));
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
          serputs(btoh(*b++));
          ozserialout(' ');
        }
        b-=16;
        for(j=0; j<16; j++,b++)
          if(*b>31 && *b<127) ozserialout(*b);
             else ozserialout('.');
        serputs("\r\n");
    }
}

void main()
{
    static char buf[120];
    static byte saved_lcr, saved_baud;
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD19200);

    ozputs(0,10,"Enter page address:");
    *buf=0;
    if(ozeditline(87,10,buf,5,100)<=0) return;
    dump(htou(buf));
    ozportout(3,7);
    ozportout(3,4);
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}

