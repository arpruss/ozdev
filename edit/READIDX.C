#include <string.h>
#include <oz.h>
#if 0
#include <ozindex.h>
#include <ozmemo.h>
#endif
#include "edit.h"

/* unsigned index_locs[MAX_INDEX_COUNT]; */
byte quick_index=0;
byte index_count;
/* byte index_lengths[MAX_INDEX_COUNT]; */

struct appindexlist ilist;
byte cur_index;
unsigned primary_sort;
unsigned secondary_sort;
unsigned total_memos,last_done;

unsigned start_extended;
extern char title[TITLE_LENGTH+1];
extern char date[9];

void beep(void)
{
    ozinitsound();
    ozsound(30);
    ozdelay64hz(4);
    ozquiet();
}

byte atob2(char *s)
{
    return (byte)((byte)((s[0]-'0'))*10)+(byte)s[1]-(byte)'0';
}

int readmemotitledate(unsigned n)
{
    static unsigned loc;

    register char *s;
    loc=ozfindrecnum(ozreadauxword2(n));
    s=ozloadcluster(loc);
    memcpy(date,s+15,9);
    memcpy(title,s+15+9,TITLE_LENGTH+1);
    return 0;
}

void find_start_extended()
{
    start_extended=total_memos;
    if(total_memos==0)
    {
        start_extended=0;
        return;
    }
    while(start_extended>0)
    {
        start_extended--;
        readmemotitledate(start_extended);
        if(strncmp(title,special_header,SPECIAL_LENGTH)
           || strcmp(date,special_date)) break;
    }
    start_extended++;
}

void preload(void)
{
    static byte i;
    for(i=0;i<PRELOAD_NUM;i++)
    {
        readmemotitledate( PRELOAD_POS(i) );
        memcpy(preload_titles[i],title,PRELOAD_LEN);
        preload_titles[i][PRELOAD_LEN]=0;
/*        printf("<%s>",preload_titles[i]);
        ozgetch();
*/
    }
}

int readmemoindex(byte number)
{
    static unsigned loc,pos;
    static int c;
    static byte i;
    static unsigned prev,cur,j;

    cur_index=number!=0;
    last_done=0;

    ozgetappindexlist(TYPENUM_MEMO,&ilist);

    if(!(loc=ozfindrecnum(ilist.indexes[cur_index])))
        return -1;
    ozopenfile(loc);
    primary_sort=ozgetw();
    secondary_sort=ozgetw();
    ozgetw();

    while(0x7f != (c=ozreadfilebyte()) && c!=-1);
    if(c==-1)
    {
        return -1;
    }
    index_count=ozreadfilebyte();
    prev=0;
    for(i=0;i<index_count;i++)
    {
        cur=ozgetw();
        index_lengths[i]=cur-prev;
        prev=cur;
    }
    total_memos=prev;
    for(i=0;i<index_count;i++)
    {
        index_locs[i]=ozgetw();
    }
    pos=0;
    for(j=0;j<index_count;j++)
    {
        cur=index_lengths[j];
        if((ozopenfile(index_locs[j]))<0)
            return -1;
        for(i=0;i<cur;i++)
        {
            ozwriteauxword(pos,ozgetw());
            ozreadfilebyte(); /* status */
            pos+=2;
        }
    }
    find_start_extended();
    preload();
    quick_index=1;
    return 0;
}

