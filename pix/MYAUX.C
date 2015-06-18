#include <oz.h>

#define EXTRABUFFERLEN 15100

byte extrabuffer[EXTRABUFFERLEN] = { '1' };

unsigned mygetauxsize(void)
{
    return EXTRABUFFERLEN+ozgetauxsize();
}

void mywriteauxmem(unsigned offset,byte *data,unsigned length)
{
    static unsigned delta;
    if(offset+length<EXTRABUFFERLEN)
    {
        memcpy(extrabuffer+offset,data,length);
        return;
    }
    if(offset>=EXTRABUFFERLEN)
    {
        ozwriteauxmem(offset-EXTRABUFFERLEN,data,length);
        return;
    }
    delta=EXTRABUFFERLEN-offset;
    memcpy(extrabuffer+offset,data,delta);
    ozwriteauxmem(0,data+delta,length-delta);
    return;
}

void myreadauxmem(unsigned offset,byte *data,unsigned length)
{
    static unsigned delta;
    if(offset+length<EXTRABUFFERLEN)
    {
        memcpy(data,extrabuffer+offset,length);
        return;
    }
    if(offset>=EXTRABUFFERLEN)
    {
        ozreadauxmem(offset-EXTRABUFFERLEN,data,length);
        return;
    }
    delta=EXTRABUFFERLEN-offset;
    memcpy(data,extrabuffer+offset,delta);
    ozreadauxmem(0,data+delta,length-delta);
    return;
}

