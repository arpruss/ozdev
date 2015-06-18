#include <string.h>
#include <oz.h>

#define INNER_SPACING 4

void _ozbox(byte x,byte y,byte ww,byte height)
{
    _ozvline(x,y,height,1);
    _ozhline(x,y,ww,1);
    _ozvline(x+ww-1,y,height,1);
    _ozhline(x,y+height-1,ww,1);
}

unsigned ozwarn(char *s,char *t)
{
#ifdef WARN_ONE_OR_TWO
    static byte numlines;
    static byte height;
#else
#   define height (18+1+12)
#endif
    static byte ww0,ww1,ww;
    static byte x,y;
#ifndef FIXED
    static byte oldfont;
#endif
    static unsigned key;
#ifndef FIXED
    oldfont=ozgetfont();
    ozsetfont(FONT_OZ_NORMAL);
#endif /* not FIXED */
    ww=ww0=ozputs(0,-1,s);
#ifdef WARN_ON_OR_TWO
    if(t!=NULL)
#endif
    {
        ww1=ozputs(0,-1,t);
        if(ww1>ww) ww=ww1;
#ifdef WARN_ONE_OR_TWO
        numlines=2;
        height=18+1+12;
#endif
    }
#ifdef WARN_ONE_OR_TWO
    else
    {
        numlines=1;
        height=8+1+12;
    }
#endif
    ww+=1+12;
    x=120-ww/2;
    y=40-height/2;
    _ozfilledbox(x+1,y+1,ww-2,height-2,WHITE);
    _ozbox(x,y,ww,height);
    _ozbox(x+2,y+2,ww-4,height-4);
    _ozbox(x+3,y+3,ww-6,height-6);
#ifdef WARN_ONE_OR_TWO
    if(numlines==1)
        ozputs(120-ww0/2,40-4,s);
    else
#endif
    {
        ozputs(120-ww0/2,40-8,s);
        ozputs(120-ww1/2,40+1,t);
    }
#ifndef FIXED
    ozsetfont(oldfont);
#endif /* not FIXED */
    key=ozgetchblank();
    if(key==KEY_UPPER_ENTER) return KEY_LOWER_ENTER;
        else return key;
}
