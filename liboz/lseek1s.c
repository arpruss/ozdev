#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4

extern enum ozfile_errornumbers _ozerrno;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
extern ozfile_t _oz_the_file;

int __read1(char *data, int length, int skip);
unsigned __ozloadpart1(unsigned part_number);
void _ozreadfirstcluster1(unsigned loc);

int rewind1(void);

extern long lseek1set(long offset)
{
    static unsigned right_part,default_part_length;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_RDONLY) return -1;
    default_part_length=_oz_the_file.first_part_length-sizeof(cfile_header_t);
    right_part=offset/default_part_length;
    if(!__ozloadpart1(right_part)) /* probably beyond EOF */
    {
        if(rewind1()<0) return -1;
        while(__read1(NULL,16384,1)==16384);
        return _oz_the_file.curpos;
    }
    _oz_the_file.curpos=right_part*(long)default_part_length;
    __read1(NULL,offset-_oz_the_file.curpos,1);
    return _oz_the_file.curpos;
}

