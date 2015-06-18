#include <string.h>
#include <oz.h>
#include "edit.h"

unsigned queued_scroll_up,queued_scroll_down;
#ifdef FIXED
extern char _60spaces[60];
#endif
extern byte changed[MAX_LINES_PER_SCREEN];
byte refresh=1;

static byte measure(byte n)
{
#ifndef FIXED
    static byte i,total;
    register char *s=line_buf;
    total=0;
    for(i=n;i>0;i--) total+=width(*s++);
    return total;
#else
    return FIXED_WIDTH*n;
#endif
}

void putline(byte n,byte first)
{
    static unsigned len;
#ifndef FIXED
    ozputsf(n);
#else /* FIXED */
    curcol=0;
    currow=n/line_height;
    ttyprintstring(line_buf);
    ttyprintstring(_60spaces+curcol);
#endif /* FIXED */
#ifndef SECURE
        if(first && (len=strlen(line_buf)))
        {
          if(len>0 && line_buf[len-1]==13) len--;
          if(len)
          {
             if(len>TITLE_LENGTH) len=TITLE_LENGTH;
             _ozhline(0,n+line_height-1,measure(len),XOR);
          }
        }
#endif
}

void toggleshowselection(void)
{
    static byte i;
    static unsigned offset,next;
    static byte start_x,end_x;
    offset=screen_offset;
    for(i=0;i<80 && offset<selection_end;i+=line_height,offset=next)
    {
        next=grab_line(offset);
        if(selection_start>=next) continue;
        if(selection_start<=offset) start_x=0;
          else start_x=measure(selection_start-offset);
        if(selection_end>=next)
          end_x=measure(next-offset);
        else end_x=measure(selection_end-offset);
        _ozfilledbox(start_x,i,end_x-start_x,font_narrow_height,XOR);
    }
}

void clear_rest(byte n)
{
    static unsigned start;
    start=n*line_height_bytes;
    if(start>=2400) return;
    cleararea(start,2400-start);
}

unsigned show_line(byte n, unsigned offset)
{
    static byte line_y;
    static unsigned next;
	line_y=n*line_height;
    if(offset<file_length)
    {
      wrap_to(offset);
      next=grab_line(offset);
      if(refresh || changed[n]) putline(line_y,offset==0);
      offset=next;
    }
    return offset;
}

void show_screen(unsigned offset)
{
	static byte i;
    screen_offset=offset;
	for(i=0;i<lines_per_screen && offset<file_length;i++)
        offset=show_line(i,offset);
    if(wrapped_length<offset) wrapped_length=offset;
    clear_rest(i);
}

void refresh_off(void)
{
    static byte i;
    if(refresh)
    {
        if(selection_on) toggleshowselection();
        refresh=0;
        for(i=0;i<lines_per_screen;i++) changed[i]=0;
        queued_scroll_up=queued_scroll_down=0;
    }
}

void refresh_on(void)
{
    if(!refresh)
    {
        fulfill_up_queue();
        fulfill_down_queue();
        show_screen(screen_offset);
        refresh=1;
        if(selection_on) toggleshowselection();
    }
}
