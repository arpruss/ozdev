#include <stdlib.h>
#include <oz.h>

main()
{
    char buf[40];
    char *s,*s1;
    int y=0;
    int x=0;
    do
    {
        s=malloc(2048);
        if(x==0 && y==0) s1=s;
        if(s==NULL)
        {
            free(s1);
            s=malloc(2048);
        }
        sprintf(buf,"Malloc'ed at %4x",s);
        ozputs(x,y,buf);
        y+=8;
        if(y>=80)
        {
            if(x) goto GETOUT; else x=120;
            y=0;
        }
    } while(NULL!=s);
GETOUT:
    ozgetch();
}


