#include <string.h>
#include <oz.h>
#include "edit.h"
#ifdef SECURE
#undef clipboard
extern char _ozfile1buffer[];
#define clipboard _ozfile1buffer
#endif

#define TRANSL

typedef void (*movefun)();
extern byte modified;

char clip_sig[6]="$cLip";

unsigned ozcrc16auxmem(unsigned start,unsigned length);

byte clip_check(void)
{
#ifndef SECURE
	if(in_clipboard+file_length>16384)
	{
        if(warn("No memory for full clipboard. Trimming.",
                yn_message)!=KEY_LOWER_ENTER) return 0;
		in_clipboard=16384-file_length;
		redraw();
	}
#endif
	return 1;
}

#ifndef SECURE
void make_clip_permanent(void)
{
	unsigned w1,pos1;
	if(!in_clipboard)
	{
		ozwriteauxword(16384-2,0);
		return;
	}
	w1=ozreadauxword(16384-2);
	ozwriteauxword(16384-2,in_clipboard);
	ozwriteauxword(pos1=16384-2-in_clipboard,w1);
	ozwriteauxmem(pos1-6,clip_sig,6);
    ozwriteauxword(pos1-8,ozcrc16auxmem(pos1,in_clipboard+2));
}
#endif

void load_clip_permanent(void)
{
	static char sig[6];
	static unsigned pos1;
	static byte c;
	if(!(in_clipboard=ozreadauxword(16384-2))
	|| in_clipboard>MAX_CLIPBOARD)
	{
		in_clipboard=0;
		return;
	}
	pos1=16384-2-in_clipboard;
	ozreadauxmem(pos1-6,sig,6);
	if(strcmp(sig,clip_sig)
#ifndef SECURE
    || ozcrc16auxmem(pos1,in_clipboard+2)!=ozreadauxword(pos1-8)
#endif
	)
	{
		in_clipboard=0;
		return;
	}
	ozwriteauxword(16384-2,ozreadauxword(pos1));
	ozwriteauxword(pos1-6,0); /* clear the clip_sig */
	for(pos1+=2;pos1<16384;pos1++)
	{
		c=ozreadauxbyte(pos1);
        if(c>127
#ifndef TRANSL
            || (c<32 && c!=13)
#endif
        )
		{
			in_clipboard=0;
			return;
		}
	}
}


void copy_clipboard(void)
{
	static unsigned i;
	register char *s;
    if(selection_end==selection_start)
		return;
	in_clipboard=selection_end-selection_start;
	if(in_clipboard>MAX_CLIPBOARD)
	{
		beep();
        warn("Too much data for clipboard. Trimming.",
        anykey_message);
		redraw();
		in_clipboard=MAX_CLIPBOARD;
	}
	selection_end=in_clipboard+selection_start;
	ozreadauxmem(selection_start,s=(char*)(clipboard),in_clipboard);
	for(i=in_clipboard;i;i--,s++)
		*s&=0x7f;
}

void goto_selection_start(void)
{
    refresh_off();
    if(cur_offset>selection_start)
    {
        cursor_tol();
        while(cur_offset>selection_start) cursor_up();
    }
    cur_offset=selection_start;
    locate_cursor();
}

void delete_selection(void)
{
	static unsigned len;
	selection_on=0;
	len=selection_end-selection_start;
	if(!len) return;
	modified=1;
    goto_selection_start();
	selection_on=0;
	save_lines(2);
	delete(cur_offset,len);
	file_length-=len;
    adjust_lines(-len,1);
	refresh_on();
}

void insert_from(char *s,unsigned len)
{
	static unsigned paste_len;
	if(!len) return;
	modified=1;
	paste_len=len;
	if(paste_len+file_length>MAX_FILE_LENGTH)
		paste_len=MAX_FILE_LENGTH-file_length;
	save_lines(2);
	insert(cur_offset,paste_len);
	file_length+=paste_len;
	wrapped_length+=paste_len;
	ozwriteauxmem(cur_offset,s,paste_len);
	cur_offset+=paste_len;
    adjust_lines(paste_len,1);
}


void paste_clipboard(void)
{
	if(!in_clipboard) return;
    ifseldel();
	modified=1;
    insert_from((char*)clipboard,in_clipboard);
}


void cut_clipboard(void)
{
	if(selection_start==selection_end || !selection_on)
	{
		return;
	}
	copy_clipboard();
	delete_selection();
}


void select_move(movefun move)
{
	refresh_off();
	if(!selection_on)
	{
		selection_on=1;
		selection_anchor=selection_end=selection_start=cur_offset;
	}
	move();
	if(cur_offset<selection_anchor)
	{
		selection_start=cur_offset;
		selection_end=selection_anchor;
	}
	else
	{
		selection_start=selection_anchor;
		selection_end=cur_offset;
	}
	refresh_on();
}

#ifndef SECURE
void save_clipboard(void)
{
	if(!in_clipboard) return;
	ozwriteauxmem(16384-in_clipboard,clipboard,in_clipboard);
}
#endif

void restore_clipboard(void)
{
	if(!in_clipboard) return;
	ozreadauxmem(16384-in_clipboard,clipboard,in_clipboard);
}


