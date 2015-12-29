#include <oz.h>

static byte x,y;

byte ozputs00( char *s)
{
    box239(0,8,0);
    setnormfont();
    return ozputs(0,0,s);
}

static unsigned long value;

void putulong()
{
    static char buffer[11];
    register char *p=buffer+10;
    *p=0;
    value=value;
    if(value==0)
    {
        x=ozputch(x,y,'0');
        return;
    }
    while(value)
    {
        p--;
        *p=value%10+'0';
        value/=10;
    }
    x=ozputs(x,y,p);
}

void putformatted(byte y0,char *s,unsigned fstring)
{
    /* | = unsigned int, ^ = unsigned long, $ = string */
    static char *p;
    register unsigned int *q=&fstring;
    x=0;
    y=y0;
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
                value=(unsigned long)*q;
                putulong();
                q++;
                break;
            case '^':
                value=(*(unsigned long*)q);
                putulong();
                q+=2;
                break;
            default:
                x=ozputch(x,y,*p);
        }
        p++;
    }
    return;
}
