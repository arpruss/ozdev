#include <string.h>
#include <oz.h>

int findownedbasicfile(char *owner)
{
    static unsigned page,offset,recnum,loc;
    register char *data;
    page=offset=recnum=0;
    while(loc=ozfindnext(TYPE_BASIC,&page,&offset,&recnum))
    {
        data=ozloadcluster(loc);
        if(data[15]=='D' && !strcmp(data+11+16,owner)) return 1;
    }
    return 0;
}

