#include <oz.h>

int ozgreyputs(int x, int y, byte shade, char *string)
{
    static int w,h;
    if(y<0) return ozputs(x,-1,string);
    if(shade!=3)
    {
        w=ozputs(0,-1,string);
        h=ozgetfontheight(ozgetfont());
    }
    ozsetactivepage(1);
    if(shade&2)
        ozputs(x,y,string);
    else
        ozfilledbox(x,y,w,h,WHITE);
    ozsetactivepage(0);
    if(shade&1)
        return ozputs(x,y,string);
    else
    {
        ozfilledbox(x,y,w,h,WHITE);
        return x+w;
    }
}

static char the_string[2]="\0\0";

int ozgreyputch(int x,int y,byte shade,char c)
{
    the_string[0]=c;
    return ozgreyputs(x,y,shade,the_string);
}
