#include <oz.h>
#include <string.h>

char *itoa(int n);

void termputs(char *s)
{
    while(*s) termputch(*s++);
}

void _sprintf(char *s, char *fmt, int n)
{
    int *data;
    data=&n;
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


void _printf(char *fmt, int n)
{
    int *data;
    data=&n;
    while(*fmt)
    {
        if(*fmt=='\n') termputs("\r\n");
        else if(*fmt=='%')
           {
             switch(*++fmt)
             {
                case '%': termputch('%'); break;
                case 's': termputs((char*)(*data)); data++; break;
                case 'c': termputch(*(char*)(data)); data++; break;
                case 'u': /* hack */
                case 'd': termputs(itoa(*data)); data++; break;
                case 0: return;
             }
           }
        else termputch(*fmt);
        fmt++;
    }
}

