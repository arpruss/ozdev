#include <oz.h>
#include <ozread.h>
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
    unsigned loc,page=0,offset=0,recnum,key;
    int f;
    int c;
    int x;
    int type;
    byte saved_lcr, saved_baud;
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozputs(0,12,"Dump erased (M)emos, (T)elephone directory date,");
    ozputs(0,22,"(S)chedule data, (A)nniversary entries, TO (D)O data,");
    x=ozputs(0,32,"or (Q)uit? ");
    switch(ozgetch())
    {
        case 'm': type=TYPE_MEMO; ozputs(x,32,"Memo"); break;
        case 't': type=TYPE_TELEPHONE; ozputs(x,32,"Telephone"); break;
        case 's': type=TYPE_SCHEDULE; ozputs(x,32,"Schedule"); break;
        case 'a': type=TYPE_ANNIVERSARY; ozputs(x,32,"Anniversary"); break;
        case 'd': type=TYPE_TODO; ozputs(x,32,"To do"); break;
        default: return;
    }
    ozputs(0,42,"Which baud rate?  (a) 9600, (b) 19200 or (c) 38400? [a]");
    switch(ozgetch())
    {
        case 'b': ozsetbaud(BAUD19200); break;
        case 'c': ozsetbaud(BAUD38400); break;
        case KEY_LOWER_ESC:
        case KEY_UPPER_ESC: return;
        default: ozsetbaud(BAUD9600); break;
    }
    ozputs(0,52,"Start data capture on PC and press a key");
    ozputs(0,62,"(ESC to abort)");
    if(KEY_LOWER_ESC==(key=ozgetch()) || KEY_UPPER_ESC==key) return;
    while(loc=ozfindnexterased(type,&page,&offset,&recnum))
    {
        if(ozkeyhit() && (KEY_LOWER_ESC==(key=ozgetch()) ||
          KEY_UPPER_ESC==key)) break;
        f=ozopenreadloc(loc);
        while(-1!=(c=ozgetc(f)))
        {
          if(ozkeyhit() && (KEY_LOWER_ESC==(key=ozgetch()) ||
          KEY_UPPER_ESC==key)) goto END;

        if(c<32 || c>126)
        {
            if(c=='\r' || c=='\n')
            {
                serputs("\r\n");
            }
            else if(c==0) ozserialout(' ');
            else if(((char)c)!='\xFF')
            {
              ozserialout('[');
              serputs(btoh(c));
              ozserialout(']');
            }
        }
        else ozserialout(c);
        }
        ozserialout('\r');
        ozserialout('\n');
        ozclose(f);
    }
END:
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}
