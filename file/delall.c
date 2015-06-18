#include <oz.h>
#include "file.h"

extern unsigned _ozcdirpages[];

main()
{
    int pos=0;
    char *firstcluster;
    register ozfile_firstpart_header_t *header;
    unsigned page,offset,recnum,loc;
    page=offset=0;
    while(loc=ozfindnext('c',&page,&offset,&recnum) && !ozkeyhit())
        ozunlink(TYPENUM_SCHEDULE,recnum);
}

