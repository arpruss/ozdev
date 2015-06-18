#include <oz.h>
#include <ozfile.h>

close(){}

main()
{
    unsigned loc;
    unsigned page=0;
    unsigned offset=0;
    int i;
    int x,y;
    char *buf;
    ozinitfs();
    loc=ozfindnext(TYPE_SCHEDULE,&page,&offset);
    if(loc==0)
    {
        ozputs(0,0,"Cannot find");
        ozgetch();
        return 0;
    }
    {
        char b[16];
        sprintf(b,"[%x]",loc);
        ozputs(0,0,b);
    }
    x=0;
    y=10;
    buf=ozloadcluster(loc);
    for(i=0;i<64; i++)
    {
        if(x>230)
        {
            y+=10;
            x=0;
        }
        if(y>70) break;
        x=ozputch(x,y,buf[i]);
    }
    ozgetch();
}
