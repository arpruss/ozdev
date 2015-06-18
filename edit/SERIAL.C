#include <oz.h>
#include "edit.h"

static void serparm(void)
{
    ozdatabits(DATABITS8);
    ozparity(NO_PARITY);
    ozstopbits(ONE_STOP_BIT);
    ozsetbaud(BAUD9600);
}

void send(void)
{
    static unsigned pos;
    static unsigned count;
    static unsigned start,end;
    static char c;
    clear_top();
    ozputs0(0,"Uploading...");
    if(selection_on)
    {
        start=selection_start;
        end=selection_end;
    }
    else
    {
        start=0;
        end=file_length;
    }
    serparm();
    count=end-(pos=start);
    while(count-- && !ozkeyhit())
    {
        c=0x7F & ozreadauxbyte(pos++);
        if(c==13)
        {
            ozserialout('\x0D');
            ozserialout('\x0A');
        }
        else ozserialout(c);
    }
    if(ozkeyhit()) ozgetch();
    redraw();
    cancel();
}

void receive(void)
{
    static unsigned maxlen,count,delta;
    static int c;
    static byte c1;
    static byte x,ct;
    ct=x=0;
    maxlen=16384-file_length;
    if(!maxlen) return;
    ifseldel();
    clear_top();
    ozputs0(0,"Downloading: any key stops...");
    serparm();
    ozserinton();
    refresh_off();
	save_lines(2);
    insert(cur_offset,count=maxlen);
    while(count && !ozkeyhit())
    {
        c=ozserialgetc();
        c1=(byte)c;
        if(c!=-1 && c1!=0xD)
        {
            c1&=0x7f;
            if(c1==0xA) c1=0xD;
                else if(c1<32) c1='*';
            ozwriteauxbyte(cur_offset++,c1);
            count--;
            if(!ct) _ozfilledbox(239,x++,1,1,XOR);
            ct++;
        }
        ozintwait();
    }
    if(ozkeyhit()) ozgetch();
    file_length+=maxlen;
    if(count)
    {
        delete(cur_offset,count);
        file_length-=count;
    }
    wrapped_length+=(delta=maxlen-count);
    modified=1;
    adjust_lines(delta,1);
    refresh_on();
    redraw();
}
