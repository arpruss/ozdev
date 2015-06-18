#include <oz.h>
#include "file.h"

byte ozflashread(unsigned p, unsigned a);
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
extern char _ozclusterbuffer[];

byte _ozkill(unsigned file_id)
{
    static unsigned loc,page,offset,recnum;
    register char *_count;
#define count ((unsigned int)_count)
    static unsigned maxcount,c;
    static byte deleted;
    maxcount=1;
    deleted=1;
    for(_count=(char*)0;deleted && count<maxcount;_count++)
    {
        page=offset=deleted=0;
        while(loc=ozfindfileid(file_id,&page,&offset,&recnum))
        {
            ozloadcluster(loc);
            c=header0.part_number+5;
            if(c > maxcount) maxcount=c;
            ozunlink(TYPENUM_SCHEDULE,recnum);
            deleted=1;
        }
    }
    return !deleted && count<maxcount;
}

int ozunlinkallowned(void)
{
    static unsigned loc,page,offset,recnum,scrap;
    static byte deleted;
    do
    {
        page=offset=deleted=0;
        while(loc=ozfindnext('c',&page,&offset,&recnum))
		{
            ozloadcluster(loc);
            if( __ozfileheaderdecode(&scrap)!=_ozfileowner ) continue;
            ozunlink(TYPENUM_SCHEDULE,recnum);
            deleted=1;
		}
    } while(deleted);
    return 0;
}

int unlink(char *name)
{
    static unsigned loc,page,offset,recnum,oldrn;
    static byte rvalue;
    extern char _ozclusterbuffer[];
    /* there might be a couple of incomplete copies of the file, and we
    want to delete them all */
    for(oldrn=page=offset=rvalue=0;(loc=ozfindfilename(name,&page,&offset,&recnum)) && recnum!=oldrn;page=offset=0)
		{
            oldrn=recnum;
            ozloadcluster(loc);
            rvalue|=_ozkill(header0.file_id);
		}
    return rvalue;
}

