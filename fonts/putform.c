#include <oz.h>

byte putu(byte x,byte y,unsigned value)
{
    static char buffer[11];
    static unsigned v;
    register char *p=buffer+10;
    *p=0;
    v=value;
    if(v==0) return ozputs(x,y,"0");
    while(v)
    {
        p--;
        *p=v%10+'0';
        v/=10;
    }
    return ozputs(x,y,p);
}

byte putformatted(byte x,byte y,char *s,unsigned fstring)
{
    /* | = unsigned int, $ = string */
    static char *p;
    register unsigned int *q=&fstring;
    p=s;
    while(*p)
    {
        switch(*p)
        {
            case '$':
                x=ozputs(x,y,*(char **)q);
                q++;
                break;
            case '|':
                x=putu(x,y,*q);
                q++;
                break;
            default:
                x=ozputch(x,y,*p);
        }
        p++;
    }
    return x;
}
