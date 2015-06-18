#include <oz.h>

#define WIDTH 232
#define HEIGHT 80

#define LWRESC   0x0101
#define LWRLEFT  0x1010
#define LWRDOWN  0x2010
#define LWRRIGHT 0x4010
#define LWRUP    0x8010
#define UPRESC   0x0108

#define NUMTREES 45
int treex[NUMTREES];
int treey[NUMTREES];
int skierx;
int thescore;
int delay;
#define SKILEN 5
#define TREESIZE 7

dodelay(sec,msec)
int sec;
int msec;
{
    int ms1,ms2;
    int i,j;
    ms1=msec%100;
    ms2=msec/100;
    for(i=0;i<ms1;i++) ozdelay(349);
    for(i=0;i<ms2;i++) ozdelay(34900);
    for(i=0;i<sec;i++)
      for(j=0;j<10;j++) ozdelay(34900);
}

lowerkeyq(key)
int key;
{
    static int l;
    l=0xff&key&ozkeylower(key>>8);
    return l;
}

upperkeyq(key)
int key;
{
    static int l;
    l=0xff&key&ozkeyupper(key>>8);
    return l;
}

fpoint(offset,x)
int x;
int offset;
{
    static int mask;
    if(offset>=2400 || offset<0 || x>=WIDTH-1) return;
    mask=1<<(x&7);
    ozdisplayorbyte(offset,mask);
}

skierchk(x,y)
int x,y;
{
    if(x<0 || y<0 || x>=WIDTH || y>=HEIGHT) return 0;
    if(x==skierx && y<SKILEN) return 1;
    return 0;
}

drawtree(i)
int i;
{
    static int j;
    static int k;
    for(j=0; j<TREESIZE; j++)
      for(k=-j; k<=j; k++)
        if(treey[i]+j<HEIGHT-1) ozpoint(treex[i]+k,treey[i]+j,1);
}

chktree(i)
int i;
{
    static int j;
    static int k;
    for(j=0; j<TREESIZE; j++)
      for(k=-j; k<=j; k++)
        if(skierchk(treex[i]+k,treey[i]+j)) return 1;
    return 0;
}

drawski(x,c)
int x,c;
{
    static int i;
    for(i=0; i<SKILEN-(c==0); i++)
    {
        if(c && ozgetpoint(x,i)) return 1;
        ozpoint(x,i,c);
    }
    return 0;
}


scrollit()
{
    static int i,j,k,x,y,offset;
    ozscroll(30);
    for(i=0; i<NUMTREES; i++)
    {
        treey[i]--;
        if(treey[i]<-TREESIZE)
        {
            treex[i]=rand()%WIDTH;
            treey[i]=HEIGHT-2;
            thescore++;
            if(thescore%10==0) delay=delay/2;
        }
        if(treey[i]>HEIGHT-2-TREESIZE)
        {
            k=(HEIGHT-2)-treey[i];
            offset=30*(HEIGHT-2);
            x=treex[i];
            for(j=-k; j<=k; j++) fpoint(offset+((x+j)/8),x+j);
        }
    }
}

putscore(s)
int s;
{
    static char score[6];
    static int i;
    for(i=0;i<5;i++) score[i]=0;
    score[5]=0;
    for(i=4; i>=0; i--)
    {
        score[i]=(s%10)+'0';
        s=s/10;
    }
    ozputs(0,HEIGHT/2,"Game over!");
    ozputs(ozputs(0,HEIGHT*3/4,"Score: "),HEIGHT*3/4,score);
}


main()
{
    static int i;
    thescore=0;
    delay=1024;
    skierx=WIDTH/2;
    ozcls();
    ozkbdoff();
    srand(ozsec()+ozmin()*60);
    for(i=0; i<NUMTREES; i++)
    {
        treex[i]=rand()%WIDTH;
 /*       treey[i]=rand()%(HEIGHT-SKILEN*2)+SKILEN*2; */
        treey[i]=rand()%HEIGHT+HEIGHT/2;
        drawtree(i);
    }
    drawski(skierx,1);
    while(1)
    {
        scrollit();
        if(lowerkeyq(LWRLEFT))
        {
            drawski(skierx,0);
            skierx--;
            if(skierx<0) skierx=0;
            if(drawski(skierx,1)) break;
        }
        else
        if(lowerkeyq(LWRRIGHT))
        {
            drawski(skierx,0);
            skierx++;
            if(skierx>=WIDTH-1) skierx=WIDTH-2;
            if(drawski(skierx,1)) break;
        }
        else
        if(lowerkeyq(LWRESC) || upperkeyq(UPRESC) || ozgetpoint(skierx,SKILEN-1)) break;
        ozpoint(skierx,SKILEN-1,1);
        for(i=0;i<delay;i++);
    }
 /*   ozsound(10); */
    putscore(thescore);
 /*
    dodelay(0,400);
    ozquiet();
    ozspkoff(); */
 /*   while(ozkeyhit()) ozgetch(); */
    while(lowerkeyq(0xFFFF));
    while(!lowerkeyq(0xFFFF));
    ozkbdon();
    ozkeyclear();
    return;
}
