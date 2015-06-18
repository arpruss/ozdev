#include <oz.h>
#include <ctype.h>

void
serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

void read_line(int x, int y, char *s)
{
    int c;
    while((c=getch())!='\n')
    {
        *s++=c;
        x=ozputch(x,y,c);
    }
    *s=0;
    return;
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

char *btoh(byte n)
{
    static char hex[3];
    hex[2]=0;
    hex[1]=hexnibble(n&0xf);
    n>>=4;
    hex[0]=hexnibble(n&0xf);
    return hex;
}



main()
{
    unsigned loc,page=0,offset=0,recnum;
    int f;
    int c;
    byte saved_lcr, saved_baud;
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD9600);
    while(loc=ozfindnext(TYPE_SYSTEM,&page,&offset,&recnum))
    {
        f=ozopenreadloc(loc);
        while(-1!=(c=ozgetc(f)))
        if(c<32 || c>126)
        {
            ozserialout('[');
            serputs(btoh(c));
            ozserialout(']');
        }
        else ozserialout(c);
        ozserialout('\r');
        ozserialout('\n');
        ozclose(f);
    }
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}
