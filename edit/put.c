#include <oz.h>
#include "edit.h"
extern char title[TITLE_LENGTH+1];
extern char date[9];
extern char typedin[TITLE_LENGTH+1];
extern byte typein;

char maintitle[]="MemoEdit";

char months[][4]={ "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                 "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

byte atob2(char *s);

void cleartop(void)
{
    cleararea(0,9*30);
}

void head(void)
{
    static byte first_head=1;
    cleartop();
    if(typein)
    {
        ozputs0(0,typedin);
    }
    else
    {
        ozputs(1,1,maintitle);
        if(first_head)
        {
            ozputs(99-7,1,"Press MENU for more information.");
            first_head--;
        }
    }
}


int putmemotitle(byte y,unsigned n)
{
    static char s[8]={0,0,0,0,0,0,0,0};
    if(readmemotitledate(n)) return -1;
    ozputs0(y,title);
    /* YYYYMMDD */
    /* 01234567 */
    if(date[6]!='0') ozputch(203,y,date[6]);
    s[0]=date[7];
    strcpy(s+1,", ");
    memcpy(s+3,date,4);
/*    s[7]=0; */
    ozputs(208,y,s);
    ozputs(180,y,months[atob2(date+4)-1]);
    return 0;
}

void showmemos(unsigned offset)
{
    static byte i;
    ozcls();
    _ozhline(0,9,239,1);
    stripe();
#ifndef FIXED
    ozfont(FONT_PC_NORMAL);
#endif /* not FIXED */
    head();
/*    if(offset>=start_extended) return; */
    for(i=0;i<7 && i+offset<start_extended;i++)
        putmemotitle(11+10*i,offset+i);
}

