#include <oz.h>

static int x,y;
static byte color;

static void drawit(char w,byte h)
{
    if(color & FILL) ozline(x+w,y+h,x+w,y-h,color);
     else
     {
        ozpoint(x+w,y+h,color);
        ozpoint(x+w,y-h,color);
     }
}

static void drawit2(char w,byte h)
{
    drawit(w,h);
    drawit(-w,h);
}

void _ozcirc25(int _x, int _y, byte r,byte _color)
{
    x=_x;
    y=_y;
    color=_color;
    switch(r)
    {
        case 2:
          drawit(0,2);
          drawit2(1,2);
          color|=FILL;
          drawit2(2,1);
          return;
        case 3:
          drawit(0,3);
          drawit2(1,3);
          drawit2(2,2);
          color|=FILL;
          drawit2(3,1);
          return;
        case 4:
          drawit(0,4);
          drawit2(1,4);
          drawit2(2,3);
          drawit2(3,3);
          if(!(color&FILL)) drawit2(3,2);
          color|=FILL;
          drawit2(4,1);
          return;
        case 5:
          drawit2(0,5);
          drawit2(1,5);
          drawit2(2,5);
          drawit2(3,4);
          drawit2(4,3);
          color|=FILL;
          drawit2(5,2);
          return;
    }
}

