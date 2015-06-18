#include <oz.h>
#include "file.h"

void __ozreadnotfirstpart(ozfile_t *_curf, unsigned loc)
{
    register ozfile_t *curf=_curf;
    memcpy(curf->buffer,&header0,62);
    if(*_ozclusterbuffer=='\xFF' && _ozclusterbuffer[1]=='\xFF')
      curf->last_cluster_of_part=1;
    else
      curf->last_cluster_of_part=0;
    curf->cur_part=header0.part_number;
    curf->last_part=header0.last_part;
    curf->cur_clusteroffset=curf->cur_partoffset=sizeof(header0);
    __ozfileheaderdecode(&(curf->part_length));
    curf->loc=loc;
}

unsigned __ozloadpart(ozfile_t *curf,unsigned part_number)
{
    static unsigned loc;
    static unsigned page,offset,recnum;
    page=offset=0;
    while((loc=ozfindfileid(curf->file_id,&page,&offset,&recnum)))
    {
        ozloadcluster(loc); /* ARP */
        if(header0.part_number != part_number)
           continue;
        __ozreadnotfirstpart(curf,loc);
        break;
    }
    return loc;
}

