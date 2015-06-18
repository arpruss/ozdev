#include <oz.h>
#include <stdlib.h>

main()
{
    int x,y,r,c,i;
    int vx,vy;
/*
    srand(ozsec()+60*ozmin());
    for(i=0;i<1;i++)
    {
        r=rand()%40;
        x=rand()%(239-2*r)+r;
        y=rand()%(80-2*r)+r;
        ozcircle(x,y,r,4+XOR);
    }
    ozgetch();
*/
    x=40;
    y=40;
    r=10;
    vx=1;
    vy=1;
    c=4;
    ozcls();
    ozsetactivepage(1);
    ozcls();
    while(1)
    {
        ozcircle(x,y,r,c+BLACK);
        for(i=0;i<2000;i++);
        if(x+vx+r>=239 || x+vx-r<0)
            vx=-vx;
        if(y+vy+r>=80 || y+vy-r<0)
            vy=-vy;
/*        x+=vx;
        y+=vy; */
        ozswapactivedisplay();
        while(!ozkeyhit());
        ozcls();
        if(ozkeyhit()) switch(ozgetch())
        {
            case 'a': if(vx>=0) vx++; if(vy>=0) vy++;
                      if(vx<0) vx--; if(vy<0) vy--;
                      break;
            case 'z': if(vx>0) vx--; if(vy>0) vy--;
                      if (vx<0) vx++; if(vy<0) vy++;
                    break;
            case KEY_UP: y--; /* if(y-r<0) y++;*/ break;
            case KEY_DOWN: y++; /* if(y+r>=80) y--; */ break;
            case KEY_LEFT: x--; /* if(x-r<0) x++; */ break;
            case KEY_RIGHT: x++; /* if(x+r>=239) x--; */ break;
            case KEY_PAGEUP: r++; /* if(x+r>=239 || y+r >=80 || y-r<0 ||
            x-r<0) r--; */ break;
            case KEY_PAGEDOWN: r--; if(r<0) r=0; break;
            case 'f': c^=4; break;
            case 'q': return;
        }
    }
}
