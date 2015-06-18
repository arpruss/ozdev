#include <oz.h>
#include "edit.h"

void do_scroll_up(byte n)
{
    static unsigned d;
    if(n>=lines_per_screen)
    {
        ozcls();
        return;
    }
    d=line_height_bytes*n;
    ozscroll(d);
    cleararea(2400-d,d);
    return;
}

void do_scroll_down(byte n)
{
    static unsigned l;
    if(n>=lines_per_screen)
    {
        ozcls();
        return;
    }
    l=line_height_bytes*n;
    ozscrolldown(l);
    cleararea(0,l);
    if((l=lines_per_screen*line_height_bytes)<2400)
        cleararea(l,2400-l);
}

void fulfill_down_queue(void)
{
    if(queued_scroll_down)
    {
        do_scroll_down(queued_scroll_down);
        queued_scroll_down=0;
    }
}

void fulfill_up_queue(void)
{
    if(queued_scroll_up)
    {
        do_scroll_up(queued_scroll_up);
        queued_scroll_up=0;
    }
}


void scrollup(void)
{
	static byte i;
    if(refresh)
    {
        do_scroll_up(1);
        return;
    }
    fulfill_down_queue();
    queued_scroll_up++;
	for(i=0;i<lines_per_screen-1;i++)
		changed[i]=changed[i+1];
    changed[lines_per_screen-1]=1;
}



void scrollup_multiple(byte n)
{
    static unsigned i;
    if(!n) return;
    if(refresh)
    {
        do_scroll_up(n);
        return;
    }
    fulfill_down_queue();
    queued_scroll_up+=n;
    if(n>=lines_per_screen)
	{
		for(i=0;i<lines_per_screen;i++)
          changed[i]=1;
		return;
	}
	for(i=0;i<lines_per_screen-n;i++)
		changed[i]=changed[i+n];
	for(;i<lines_per_screen;i++)
        changed[i]=1;
}

void scrolldown(void)
{
    static unsigned i;
    if(refresh)
    {
        do_scroll_down(1);
        return;
    }
    fulfill_up_queue();
    queued_scroll_down++;
	for(i=lines_per_screen-1;i>0;i--)
		changed[i]=changed[i-1];
    changed[0]=1;
}

void scrolldown_multiple(byte n)
{
	static unsigned i;
    if(!n) return;
    if(refresh)
    {
        do_scroll_down(n);
        return;
    }
	if(refresh) return;
    fulfill_up_queue();
    queued_scroll_down+=n;
    for(i=lines_per_screen-1;i>=n;i--)
		changed[i]=changed[i-n];
	for(;i>0;i--)
        changed[i]=1;
    changed[0]=1;
}

