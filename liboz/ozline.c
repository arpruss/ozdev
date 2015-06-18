#include <oz.h>

void ozline(int _x1,int _y1,int _x2,int _y2,byte color)
{
static int temp;
static int dx,dy;
static int x1,y1,x2,y2;
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
    if(x1<0 || x1>238 || y2<0 || y1>79) return;
    if(y1<0) y1=0;
    if(y2>79) y2=79;
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
    if(y1<0 || y1>79 || x2<0 || x1>238) return;
    if(x1<0) x1=0;
    if(x2>238) x2=238;
    _ozhline(x1,y1,x2-x1+1,color);
    return;
}
else
{
dx=x2-x1;
ozpoint(x1,y1,color);

if (y1>y2)
    {
    dy=y1-y2;
    if (dx>=dy)
     {
        temp=(dx>>1) - dx;
     for (x1++;x1<=x2;x1++)
         {
         temp+=dy;
         if (0<temp)
         {
          y1--;
          temp-=dx;
         }
         ozpoint(x1,y1,color);
         }
     }
    else
     {
     temp=(dy>>1) - dy;
     for (y1--;y1>=y2;y1--)
        {
         temp+=dx;
         if (0<temp)
          {
            x1++;
          temp-=dy;
         }
         ozpoint(x1,y1,color);
         }
     }
    }
else
    {
    dy=y2-y1;
    if (dx>=dy)
     {
        temp=(dx>>1)-dx;
     for (x1++;x1<=x2;x1++)
         {
         temp+=dy;
         if (0<temp)
         {
          y1++;
          temp-=dx;
         }
         ozpoint(x1,y1,color);
         }
     }
    else
     {
        temp=(dy>>1)-dy;
     for (y1++;y1<=y2;y1++)
         {
         temp+=dx;
         if(0<temp)
          {
          x1++;
          temp-=dy;
          }
         ozpoint(x1,y1,color);
         }
     }
    }
  }
}


