#include <oz.h>

void _ozline(byte _x1,byte _y1,byte _x2,byte _y2,byte color)
{
static int temp;
static int dx,dy;
static char y1,y2;
static byte x1,x2;
x1=_x1;
y1=_y1;
x2=_x2;
y2=_y2;

if(x1==x2)
{
    if(y2<y1)
    {
       temp=y1; y1=y2; y2=temp;
    }
    _ozvline(x1,y1,y2-y1+1,color);
    return;
}
else
if (x1>x2)
    {
    temp=x1;
    x1=x2;
    x2=temp;
    temp=y1;
    y1=y2;
    y2=temp;
}

if(y1==y2)
{
    _ozhline(x1,y1,x2-x1+1,color);
    return;
}
else
{
dx=x2-x1;

if (y1>y2)
    {
    dy=y1-y2;
    if (dx>=dy)
     {
        temp=-dx;
     for (;x1<=x2;x1++)
         {
         temp=temp+dy;
         if (0<=temp)
         {
          y1--;
          temp-=dx;
         }
         _ozpoint(x1,y1,color);
         }
     }
    else
     {
     temp=-dy;
     for (;y1>=y2;y1--)
        {
         temp+=dx;
         if (0<=temp)
          {
            x1++;
          temp-=dy;
         }
         _ozpoint(x1,y1,color);
         }
     }
    }
else
    {
    dy=y2-y1;
    if (dx>=dy)
     {
        temp=-dx;
     for (;x1<=x2;x1++)
         {
         temp+=dy;
         if (0<=temp)
         {
          y1++;
          temp-=dx;
         }
         _ozpoint(x1,y1,color);
         }
     }
    else
     {
        temp=-dy;
     for (;y1<=y2;y1++)
         {
         temp+=dx;
         if(0<=temp)
          {
          x1++;
          temp-=dy;
          }
         _ozpoint(x1,y1,color);
         }
     }
    }
  }
}


