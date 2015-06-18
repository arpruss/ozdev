#include <oz.h>
#include <ctype.h>
#include "ozread.h"

extern char LNM;

void paste_to_serial(void)
{
    static unsigned page,offset,loc,handle,recnum;
    static unsigned i;
    static unsigned paste_len;
    static byte c;
    page=offset=0;
    while(loc=ozfindnext(TYPE_SYSTEM,&page,&offset,&recnum))
    {
        if('\xF'!=(c=ozloadcluster(loc)[15]) && '!'!=c) continue;
          /* '!' is on 700M model */
        handle=ozopenreadloc(loc);
        for(i=0;i<15;i++) ozgetc(handle);
        paste_len=ozgetc(handle);
        paste_len+=256*ozgetc(handle);
        if(paste_len <= 6)
        {
            ozclose(handle);
            return;
        }
        paste_len-=6;
        ozgetc(handle);
        ozgetc(handle);
        for(i=0;i<paste_len;i++)
        {
            int c=ozgetc(handle);
            if(-1==c)
            {
                ozclose(handle);
                return;
            }
            else if(c==0)
                ozserialout_echo(' ');
            else if(c=='\x0D')
            {
                ozserialout_echo('\r');
                if(LNM) ozserialout_echo('\n');
            }
            else if(!isprint(c))
                ozserialout_echo('.');
            else ozserialout_echo(c);
        }
        ozclose(handle);
        return;
    }
    beep();
}

