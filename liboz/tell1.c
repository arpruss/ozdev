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

long tell1(void)
{
    return _oz_the_file.curpos;
}

