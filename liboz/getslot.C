#include <oz.h>
#include <string.h>

#define NAMELEN 12

extern char _ozfilename[];

byte ozgetmyslot(void)
{
    static byte page,i,j,offset;
    register char *p;
    if(ozdetectmodel() & MODELMASK_M) return 0;
    for(i=1, page=0x60; i<=10; i++, page+=4)
    {
        offset=1+ozgetmembyte(page,8)+(ozgetmembyte(page,9)<<8);
        p=_ozfilename;
        for(j=0;j<NAMELEN;j++,offset++,p++)
            if(*p != (char)ozgetmembyte(page,offset)) break;
        if(j==NAMELEN)
            return i;
    }
    return 0;
}

