#include <string.h>
#include <oz.h>
#include "edit.h"

byte memo[HEADER+MAX_MEMO_LENGTH+1];
char special_header[SPECIAL_LENGTH+1]="zzzz\xFD!";
char special_date[]="20991231";

main()
{
    static unsigned k;
    static unsigned loc,page,offset,recnum;
    static int c;
    static unsigned i;
    static byte x;
    static unsigned processed;
    static char message[8];
    ozfont(FONT_OZ_LARGE);
    ozputs(0,10,"This cleans up screwed up dates");
    ozputs(0,23,"on extended-data MemoEdit memos.");
    ozputs(0,40,"Do you wish to continue?");
    ozputs(0,53,"Yes:[ENTER]/No:[ESC]");
    k=ozgetch();
    if(k!=KEY_LOWER_ENTER && k!=KEY_UPPER_ENTER) return;
    page=offset=0;
    processed=0;
    ozcls();
    x=ozputs(0,0,"Processing... ");
    ozputs(0,40,"Press ESC to interrupt");
    while(loc=ozfindnext(TYPE_MEMO,&page,&offset,&recnum))
    {
        ozopenfile(loc);
        if(ozkeyhit())
        {
            k=ozgetch();
            if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC) return;
        }
        for(i=0;i<HEADER;i++)
            memo[i]=ozreadfilebyte();
        if(strncmp((char*)memo+TITLE_OFFSET,special_header,SPECIAL_LENGTH) ||
         !strcmp((char*)memo+DATE_OFFSET,special_date)) continue;
        processed++;
        sprintf(message,"%u",processed);
        ozputs(0,x,message);
        strcpy((char*)memo+DATE_OFFSET,special_date);
        while(-1!=(c=ozreadfilebyte()) && c && i<HEADER+MAX_MEMO_LENGTH)
            memo[i++]=c;
        if(!c) memo[i++]=c;
        ozclosefile();
        ozunlink(TYPENUM_MEMO,recnum);
        ozwritefile(memo,i,TYPENUM_MEMO);
        page=offset=0;
    }
}
