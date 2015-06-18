#include <string.h>
#include <ozgfx.h>
#include <ozinput.h>
#include "edit.h"

extern byte cursor_y;
static byte x,y1,pos,x0;
static byte xlen;
static char *s;
static byte height;

unsigned putit(void)
{
    _ozfilledbox(x0,y1,xlen,height,0);
    return ozputs(x0,y1,s);
}

static void xorcursor()
{
    static byte store_x,store_lh;
    static unsigned store_lhb;
    static byte state=0;
    extern byte display_x;
    store_x=display_x;
    store_lh=line_height;
    store_lhb=line_height_bytes;
    display_x=x;
    cursor_y=y1;
    line_height=ozgetfontheight(ozgetfont());
    line_height_bytes=30*line_height;
    if(state^=1)
        do_cursor_on();
    else
        cursor_off();
    display_x=store_x;
    line_height=store_lh;
    line_height_bytes=store_lhb;
}

static void setxorcursor(void)
{
     static byte c;
     register char *p;
     p=s+pos;
     c=*p;
     *p=0;
     x=x0+measurestring(s);
     *p=c;
     xorcursor();
}

static void movecursor(void)
{
     xorcursor();
     setxorcursor();
}

int ozeditline1(byte _x0,byte y0,char *s0,byte slen,byte _xlen)
{
    static char c;
    static byte l1;
    static unsigned k;
    static byte i;
    register char *p;
    xlen=_xlen;
    /*
    switch(ozgetfont())
    {
        case FONT_PC_NORMAL:
        case FONT_OZ_NORMAL:
            height=8;
            break;
        default:
            height=13;
            break;
    }
    */
    height=ozgetfontheight(ozgetfont());
    x=x0=_x0;
    y1=y0;
    if((int)x0+(int)xlen>239 || (int)y1+(int)height>79)
        return OZEDITLINE_ERROR;
    xlen--;
    s=s0;
    pos=strlen(s0);
    if(pos>=slen) return OZEDITLINE_ERROR;
    putit();
    setxorcursor();
    for(;;)
        switch(k=getch())
        {
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            case KEY_LEFT:
              if(pos>0)
              {
                 pos--;
                 movecursor();
              }
              break;
            case KEY_RIGHT:
              if(s[pos])
              {
                 pos++;
                 movecursor();
              }
              break;
            case KEY_MAIN:
            case KEY_SCHEDULE:
            case KEY_MEMO:
            case KEY_TELEPHONE:
            case KEY_MYPROGRAMS:
            case KEY_UPPER_ESC:
            case 27:
              xorcursor();
              return OZEDITLINE_CANCEL;
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
/*            case '\r': */
            case '\n':
                 xorcursor();
                 return strlen(s);
            case KEY_BACKSPACE:
              if(pos>0)
              {
                 pos--;
                 p=s+pos;
                 c=*p;
                 while(*p)
                 {
                    *p=p[1];
                    p++;
                 }
                 xorcursor();
                 _ozfilledbox(putit(),y1,measurechar(c),height,WHITE);
/*                 putit(); */
                 setxorcursor();
               }
               break;
            default:
               if(k<127 && (byte)measurechar(k)+(byte)measurestring(s) < xlen
                 && (l1=strlen(s)+1) < slen)
               {
                 xorcursor();
                 for(i=l1;i>pos;i--)
                    s[i]=s[i-1];
/*                 if(s[pos]==0) s[pos+1]=0; */
                 s[pos]=k;
                 pos++;
                 putit();
                 setxorcursor();
               }
               break;
          }
}

