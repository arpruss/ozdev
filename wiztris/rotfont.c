#include <oz.h>

byte rotated_font[128][9]=
{
#include "rotfont.h"
};

byte putrotated(int col,byte line,char *_s)
{
    static unsigned pos;
    static byte i,h;
    register char *s=_s;
    if(col<0)
    {
        while(*s)
        {
            line+=rotated_font[*s][0];
            s++;
        }
        return line;
    }
    pos=col+((unsigned int)line)*30;
    while(*s)
    {
        line+=(h=rotated_font[*s][0]);
        for(i=0;i<h;i++)
        {
          ozdisplayputbyte(pos,rotated_font[*s][i+1]);
          pos+=30;
        }
        s++;
    }
    return line;
}


