#include <oz.h>
#include <ozread.h>

close(){}

main()
{
    int h;
    int c;
    int x;
    int y;
    char buf[45];
    unsigned page,offset;
    h=ozopenread("Test",&page,&offset);
    if(h==-1)
    {
        ozputs(0,10,"Cannot open");
        ozgetch();
        return;
    }
    x=0;
    y=10;
    while(-1!=(c=ozgetc(h)))
    {
        if(x>230)
        {
            y+=10;
            x=0;
        }
        if(y>70) break;
        x=ozputch(x,y,c);
    }
    ozgetch();
}
