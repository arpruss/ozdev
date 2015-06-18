#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4


extern enum ozfile_errornumbers _ozerrno;
extern ozfile_t _ozfilelist[MAX_FILES];
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];

int __read(int handle, char *data, int length, int skip);
unsigned __ozloadpart(ozfile_t *curf,unsigned part_number);

extern long lseek(int handle, long offset, int fromwhere)
{
    register ozfile_t *curf;
    static unsigned right_part,default_part_length;
    if(handle>=MAX_FILES || handle<0) return -1;
    curf=_ozfilelist+handle;
    if(NULL==curf->buffer || curf->mode != O_RDONLY) return -1;
    if(fromwhere==SEEK_CUR && offset==0) return curf->curpos;
    if(fromwhere==SEEK_CUR)
        offset+=curf->curpos;
    else if(fromwhere==SEEK_END)
    {
        while(__read(handle,NULL,16384,1)==16384);
        offset+=curf->curpos;
    }
    if(offset<0)
    {
        if(rewind(handle)>=0) return 0; else return -1;
    }
    default_part_length=curf->first_part_length-sizeof(cfile_header_t);
    right_part=offset/default_part_length;
    if(!__ozloadpart(curf,right_part)) /* probably beyond EOF */
    {
        if(rewind(handle)<0) return -1;
        while(__read(handle,NULL,16384,1)==16384);
        return curf->curpos;
    }
    curf->curpos=right_part*(long)default_part_length;
    __read(handle,NULL,offset-curf->curpos,1);
    return curf->curpos;
}

