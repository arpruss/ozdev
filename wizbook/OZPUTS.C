#include <oz.h>
#include "wizbook.h"

extern byte last_height;
extern byte ozputsf_start_pos;
extern byte ozputsf_clear;

int ozputs(int x,int y,char *s)
{
    byte out;
    byte rev;
    rev=dorev;
    dorev=0;
    ozputsf_start_pos=x;
    strncpy(line_buf,s,250);
    line_buf[250]=0;
    ozputsf_clear=0;
    out=ozputsf(y);
    ozputsf_start_pos=0;
    ozputsf_clear=1;
    dorev=rev;
    return out;
}

int ozputch(int x,int y,char c)
{
    static char line[2]="\0\0";
    line[0]=c;
    return ozputs(x,y,line);
}
