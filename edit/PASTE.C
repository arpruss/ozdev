#include <oz.h>

#include "edit.h"

void pastein(void)
{
    static unsigned page,offset,loc,recnum;
    static unsigned i;
    static unsigned paste_len,pos;
    static byte c;
    page=offset=0;
    while(loc=ozfindnext(TYPE_SYSTEM,&page,&offset,&recnum))
    {
        if('\xF'!=(c=ozloadcluster(loc)[15]) && '!'!=c) continue;
          /* '!' is on 700M model */
        ozopenfile(loc);
        for(i=0;i<15;i++) ozreadfilebyte();
        paste_len=ozreadfilebyte();
        paste_len+=256*ozreadfilebyte();
        if(paste_len<=6)
        {
            ozclosefile();
            return;
        }
        paste_len-=6;
        if(paste_len+file_length>MAX_FILE_LENGTH)
          paste_len=MAX_FILE_LENGTH-file_length;
        ozreadfilebyte();
        ozreadfilebyte();

/*        refresh_off(); */

        save_lines(2);
        insert(cur_offset,paste_len);
        file_length+=paste_len;
        wrapped_length+=paste_len;
        pos=cur_offset;
        cur_offset+=paste_len;

        for(i=0;i<paste_len;i++,pos++)
        {
            int c=ozreadfilebyte();
            if(-1==c)
            {
                ozclosefile();
                return;
            }
            else if(c==0)
                ozwriteauxbyte(pos,' ');
/*                insert_char(' '); */
            else if(c=='\x0D')
                ozwriteauxbyte(pos,13);
/*                insert_char(13); */
            else if(c<32 || c>126)
                ozwriteauxbyte(pos,'*');
/*                insert_char('.'); */
            else
                ozwriteauxbyte(pos,c);
/*                insert_char(c); */
        }
        ozclosefile();
        adjust_lines(paste_len,1);
/*        refresh_on(); */
        return;
    }
}

