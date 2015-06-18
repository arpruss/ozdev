#include <oz.h>
#include <string.h>

char *itoa(int _u)
{
    static char buffer[10];
    static int u;
    register char *b;
    char neg=0;
    u=_u;
    b=buffer+9;
    *b=0;
    if(u<0)
    {
        u=-u;
        neg=1;
    }

    while(u || b==buffer+9)
    {
        *--b=(unsigned char)(u%10)+(unsigned char)'0';
        u/=10;
    }
    if(neg)
    {
        *--b='-';
    }
    return b;
}


void _sprintf(char *_s, char *_fmt, int n)
{
    register int *data;
    static char *s,*fmt;
    data=&n;
    s=_s;
    fmt=_fmt;
    while(*fmt)
    {
        if(*fmt=='%')
           {
             switch(*++fmt)
             {
                case '%': *s++='%'; break;
                case 's': strcpy(s,(char*)(*data)); s+=strlen(s);
                    data++; break;
                case 'c': *s++=*(char*)(data); data++; break;
                case 'd': strcpy(s,itoa(*data)); s+=strlen(s); data++;
                    break;
                case 0: *s=0; return;
             }
           }
        else
            *s++=*fmt;
        fmt++;
    }
    *s=0;
}

