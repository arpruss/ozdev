#include <string.h>
#include <ctype.h>
#include <oz.h>
#include "ozread.h"
#include "exp.h"

#define MAXENTRIES 3000  /* is supposed to be 5000 */

typedef char schar;  /* signed char */

unsigned exps[MAXENTRIES];
unsigned numexps;

schar cat_to_send=0;

#define NUMCATS 13

char categories[NUMCATS][20];

char *statuses[4]
= { "To be submitted", "Submitted", "Reimbursed", "None" };

int
mycompare(void *_e1, void *_e2)
/* appts_t *e1, appts_t *e2) */
#define e1 (*(unsigned *)_e1)
#define e2 (*(unsigned *)_e2)
{
    char buf1[64];
    if(e1==e2) return 0;
    strcpy(buf1,17+ozloadcluster(e1));
    return strcmp(buf1,17+ozloadcluster(e2));
}
#undef e1
#undef e2

void load_exps(void)
{
    unsigned page,offset,loc,arg3;
    numexps=page=offset=0;
    while(loc=ozfindnext(TYPE_EXPENSES,&page,&offset,&arg3))
    {
        if(cat_to_send && cat_to_send!=ozloadcluster(loc)[15]) continue;
        exps[numexps++]=loc;
        if(numexps>=MAXENTRIES) return;
    }
}

void load_cats(void)
{
    static unsigned page,offset,loc,handle,arg3;
    static byte i,j;
    static byte id,model;
    page=offset=0;
    model=ozdetectmodel();
    if((model & MODELMASK_M) || (model & MODELMASK_770))
       id=0x17;
    else
       id=0x5;
    while(loc=ozfindnext(TYPE_SYSTEM,&page,&offset,&arg3))
    {
        if(id!=ozloadcluster(loc)[15]) continue;
        handle=ozopenreadloc(loc);
        for(i=0;i<15+20;i++) ozgetc(handle);
        for(i=0;i<NUMCATS;i++)
         for(j=0;j<20;j++)
          categories[i][j]=ozgetc(handle);
        ozclose(handle);
        return;
    }
    ozputs(10,0,"Could not locate categories file...");
}

void ozreadn(char *s, unsigned n, unsigned handle)
{
    while(n--) *s++=ozgetc(handle);
}

void serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

void serputs_doublequotes(char *s)
{
    ozserialout('"');
    while(*s)
    {
        if((*s)=='"') ozserialout('"');
        ozserialout(*s++);
    }
    ozserialout('"');
}

char *utoa(unsigned u)
{
    static char buffer[10];
    register char *b;
    b=buffer+9;
    *b=0;
    while(u || b==buffer+9)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    return b;
}

emit_bcd(bcd x)
{
    byte i;
    for(i=0;i<BCD_DIGITS/2;i++)
    {
      if(x.decimalpoint==0)
         ozserialout('.');
      ozserialout('0'+(x.digits[i]>>4));
      x.decimalpoint--;
      if(x.decimalpoint<=-2) break;
      if(x.decimalpoint==0)
      {
         ozserialout('.');
      }
      ozserialout('0'+(0x0F&x.digits[i]));
      x.decimalpoint--;
      if(x.decimalpoint<=-2) break;
    }
    if(x.decimalpoint==0) ozserialout('.');
    while(x.decimalpoint-- > -2) ozserialout('0');
}

void emit_record(unsigned loc)
{
    struct expense_entry_head h;
    int c;
    byte submit_status;
    int handle=ozopenreadloc(loc);
    if(handle==-1)
    {
        serputs("Error!");
        return;
    }
    ozreadn((char *)&h,sizeof(h),handle);
    ozserialout(h.month[0]);
    ozserialout(h.month[1]);
    ozserialout('/');
    serputs(h.day);
    ozserialout('/');
    ozserialout(h.year[0]);
    ozserialout(h.year[1]);
    ozserialout(h.year[2]);
    ozserialout(h.year[3]);
    ozserialout(',');
    serputs_doublequotes(categories[h.category-1]);
    ozserialout(',');
    emit_bcd(h.amount);
    ozserialout(',');
    serputs_doublequotes(h.payment_type);
    serputs(",\"");
    while(-1!=(c=ozgetc(handle)) && c)
    {
      if(c=='"') ozserialout('"');
      ozserialout(c);
    }
    serputs("\",");
    submit_status=ozgetc(handle);
    serputs(statuses[submit_status-1]);
    serputs("\r\n");
    ozclose(handle);
}

main()
{
    unsigned i;
    int c;
    int x,y;
    byte saved_lcr, saved_baud;
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD9600);
    ozcls();
    ozputs(0,0,"Loading categories...");
    load_cats();
    ozputs(0,0,"Choose category to transmit");
    for(i=0;i<=NUMCATS;i++)
    {
        if(i<(NUMCATS+1)/2)
        {
            x=0;
            y=10+10*i;
        }
        else
        {
            x=120;
            y=10+10*(i-(NUMCATS+1)/2);
        }
        x=ozputch(x,y,i<10?i+'0':i+'A'-10);
        x=ozputs(x,y,". ");
        ozputs(x,y,i?categories[i-1]:"* All *");
    }
    c=ozgetch();
    if(isdigit(c)) cat_to_send=c-'0';
     else if(isalpha(c)) cat_to_send=c-'a'+10;
     else cat_to_send=0;
    if(cat_to_send>NUMCATS) cat_to_send=0;
    ozcls();
    x=ozputs(0,0,"Will transfer ");
    ozputs(x,0,cat_to_send ? categories[cat_to_send-1] : "* All *");
    ozputs(0,20,"Loading data...");
    load_exps();
    ozputs(0,30,"Sorting...");
    qsort((void *)(exps),numexps,sizeof(exps[0]),mycompare);
    ozputs(0,40,"Start capture on PC and press any key to emit.");
    ozgetch();
    serputs("Date,Category,Amount,Payment Method,Memo,Submission Status\r\n");
    for(i=0;i<numexps;i++)
      emit_record(exps[i]);
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
}

