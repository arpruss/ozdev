#include <oz.h>
#include "file.h"

main()
{
    extern char datum, datum2;
    int pos=0;
    char *firstcluster;
    static char buf[25];
    register ozfile_firstpart_header_t *header;
    unsigned page,offset,recnum,loc;
    page=offset=0;
    while(loc=ozfindfileid(0x0B00,&page,&offset,&recnum))
    {
        firstcluster=ozloadcluster(loc);
        header=(ozfile_firstpart_header_t *)(2+firstcluster);
        ozputs(0,pos,(char*)&(header[1]));
        sprintf(buf,"%04x %02x %02x",(header->h1).file_id,datum,datum2);
        ozputs(120,pos,buf);
        pos+=8;
        if(pos>=72)
        {
            ozputs(0,pos,"Press any key to continue");
            ozgetch();
            ozcls();
            pos=0;
        }
    }
    ozgetch();
}

