#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"
#include "cache.h"

#define O_CHECKONLY 4

extern enum ozfile_errornumbers _ozerrno;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
extern ozfile_t _oz_the_file;

int __read1c(char *data, int length, int skip);
unsigned __ozloadpart1c(unsigned part_number);
void _ozreadfirstcluster1c(unsigned loc, char *firstcluster);

int rewind1c(void)
{
    if(NULL==_oz_the_file.buffer || !(_oz_the_file.mode & O_RDONLY)) return -1;
    _ozreadfirstcluster1c(_oz_the_file.startloc,ozloadcluster(_oz_the_file.startloc));
    return 0;
}

long lseek1c(long offset, int fromwhere)
{
    static unsigned right_part,default_part_length;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_RDONLY) return -1;
    if(fromwhere==SEEK_CUR && offset==0) return _oz_the_file.curpos;
    if(fromwhere==SEEK_CUR)
        offset+=_oz_the_file.curpos;
    else if(fromwhere==SEEK_END)
    {
        while(__read1c(NULL,16384,1)==16384);
        offset+=_oz_the_file.curpos;
    }
    if(offset<0)
    {
        if(rewind1c()>=0) return 0; else return -1;
    }
    default_part_length=_oz_the_file.first_part_length-sizeof(cfile_header_t);
    right_part=offset/default_part_length;
    if(!__ozloadpart1c(right_part)) /* probably beyond EOF */
    {
        if(rewind1c()<0) return -1;
        while(__read1c(NULL,16384,1)==16384);
        return _oz_the_file.curpos;
    }
    _oz_the_file.curpos=right_part*(long)default_part_length;
    __read1c(NULL,offset-_oz_the_file.curpos,1);
    return _oz_the_file.curpos;
}

