#include <stdio.h>
#include <string.h>
#include <oz.h>

char *itoa(int a);

void termputch(char c);

void termputs(char *s)
{
    while(*s) termputch(*s++);
}

static char buffer[256];

void ozgets(char *buf)
{
    static int curpos;
    static unsigned c;
    curpos=0;
    while('\n'!=(c=getch()))
    {
        if(c==8)
        {
            termputch(8);
            termputch(' ');
            curpos--;
        }
        else if(c==KEY_MYPROGRAMS)
        {
            exit(0);
        }
        else buf[curpos++]=c;
        termputch(c);
    }
    termputch('\n');
    termputch('\r');
    buf[curpos]=0;
}

/*
int ozscanf(char *fmt, int args)
{
    static FILE    file;
    ozgets(buffer);

    file._base = file._ptr = buffer;
    file._cnt = strlen(buffer);
    file._flag = _IOSTRG|_IOBINARY|_IOREAD;
    return _doscan(&file, fmt, &args);
}

int termprintf(char *fmt, int args)
{
    static FILE spf;
    register char *p;
    spf._cnt = 32767;
    spf._ptr = buffer;
    spf._flag = _IOWRT|_IOBINARY|_IOSTRG;
    _doprnt(&spf, fmt, &args);
    *spf._ptr = 0;
    p=buffer;
    while(*p) termputch(*p++);
    return spf._ptr - buffer;
} */

int ozgetint(int *a)
{
    ozgets(buffer);
    if(!isdigit(*buffer) && *buffer!='-') return 0;
    *a=atoi(buffer);
    return 1;
}

void ozputint(int n)
{
    char *p;
    p=itoa(n);
    while(*p) termputch(*p++);
}
