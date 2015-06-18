#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4


extern enum ozfile_errornumbers _ozerrno;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
extern ozfile_t _oz_the_file_ro;

int __read1ro(char *data, int length, int skip);
unsigned __ozloadpart1ro(unsigned part_number);
void _ozreadfirstcluster1(unsigned loc, char *firstcluster);

int rewind1ro(void)
{
    if(NULL==_oz_the_file_ro.buffer || !(_oz_the_file_ro.mode & O_RDONLY)) return -1;
    _ozro1readfirstcluster(_oz_the_file_ro.startloc,ozloadcluster(_oz_the_file_ro.startloc));
    return 0;
}

extern long lseek1ro(long offset, int fromwhere)
{
    static unsigned right_part,default_part_length;
    if(NULL==_oz_the_file_ro.buffer || _oz_the_file_ro.mode != O_RDONLY) return -1;
    if(fromwhere==SEEK_CUR && offset==0) return _oz_the_file_ro.curpos;
    if(fromwhere==SEEK_CUR)
        offset+=_oz_the_file_ro.curpos;
    else if(fromwhere==SEEK_END)
    {
        while(__read1ro(NULL,16384,1)==16384);
        offset+=_oz_the_file_ro.curpos;
    }
    if(offset<0)
    {
        if(rewind1ro()>=0) return 0; else return -1;
    }
    default_part_length=_oz_the_file_ro.first_part_length-sizeof(cfile_header_t);
    right_part=offset/default_part_length;
    if(!__ozloadpart1ro(right_part)) /* probably beyond EOF */
    {
        if(rewind1ro()<0) return -1;
        while(__read1ro(NULL,16384,1)==16384);
        return _oz_the_file_ro.curpos;
    }
    _oz_the_file_ro.curpos=right_part*(long)default_part_length;
    __read1ro(NULL,offset-_oz_the_file_ro.curpos,1);
    return _oz_the_file_ro.curpos;
}

