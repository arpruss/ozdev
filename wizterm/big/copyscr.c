#include <oz.h>
#include <string.h>
#include "memo.h"

char *utoa_0n(unsigned n, unsigned digits);

void copy_the_screen()
{
    extern unsigned screenmemolen;
    static unsigned year,month,day,hour,minute,second;
    register char *s;
    year=ozyear();
    month=ozmonth();
    day=ozday();
    hour=ozhour();
    minute=ozmin();
    second=ozsec();
    s=scan_screen();
    strcpy(s+DATE_OFFSET,utoa_0n(year,4));
    strcpy(s+DATE_OFFSET+4,utoa_0n(month,2));
    strcpy(s+DATE_OFFSET+6,utoa_0n(day,2));
    strcpy(s+TITLE_OFFSET,"Wizterm at ");
    strcat(s+TITLE_OFFSET,utoa_0n(hour,2));
    strcat(s+TITLE_OFFSET,":");
    strcat(s+TITLE_OFFSET,utoa_0n(minute,2));
    strcat(s+TITLE_OFFSET,":");
    strcat(s+TITLE_OFFSET,utoa_0n(second,2));  /* title should be exactly 20
                                           characters long, space padded
                                           if necessary */
    ozwritefile(s,screenmemolen,MEMO_WRITE_TYPE);
}

