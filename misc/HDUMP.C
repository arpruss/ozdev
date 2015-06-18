#include <oz.h>

void
serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

void main()
{
    static char buf[80];
    register byte *b=(byte *)0xa000;
    static unsigned i;
    static byte saved_lcr;
    static byte saved_baud;
    saved_lcr=ozgetlcr(),
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD9600);
    serputs("HDUMP dumping\r\n");
    for(i=9; i<0x180; i++)
    {
        ozportout(3,i&0xFF);
        ozportout(4,i>>8);
        sprintf(buf,"%04x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",i,
          b[0],b[1],b[2],b[3],b[4],b[5],b[6],b[7]);
        serputs(buf);
    }
    ozportout(3,7);
    ozportout(3,4);
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}

