#include <oz.h>
#include <string.h>
#include "file.h"

extern unsigned _ozfileowner;

char *ozgetnextfilename(byte mode)
{
    static unsigned page=0,offset=0;
    static unsigned loc,recnum;
    static char name[MAX_FILENAMELEN+1];
    static unsigned length;
    if(mode & FIND_RESET)
        page=offset=0;
    while(loc=ozfindnext('c',&page,&offset,&recnum))
    {
        ozloadcluster(loc);
        if(header0.part_number || ( (mode&FIND_OWNED)
          && _ozfileowner!=__ozfileheaderdecode(&length))) continue;
        strcpy(name,header0.filename);
        return name;
    }
    return NULL;
}
