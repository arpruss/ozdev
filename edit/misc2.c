#include <string.h>
#include <oz.h>
#include "edit.h"

/* #define THIN_CURSOR /* single-line cursor */
#define FLASH_RATE 16 /* also change in ozcustom.as */
#define EXTENDED_CURSOR /* extended cursor */

extern char sstring[];

void locate_found(unsigned last_found)
{
	refresh_off();
	cursor_tof();
	cur_offset=last_found;
	locate_cursor();
	refresh_on();
/*	  show_screen(screen_offset); */
}

void clear_top(void)
{
#ifdef FIXED
    cleararea(0,8*30);
#else
    cleararea(0,line_height_bytes);
#endif
}

void edit_search(void)
{
	static byte x; static byte l;
    clear_top();
	x=ozputs(0,0,sstring);
	if(ozeditline1(x,0,search_string,SEARCH_STRING_MAX,238-x)
	 <= 0 || (l=strlen(search_string))>file_length)
	{
		redraw();
		return;
	}
    last_found=search(0,l);
	redraw();
	if(last_found==0xFFFF)
	{
		beep();
		return;
	}
    locate_found(last_found);
}

void edit_search_again(void)
{
	static byte l;
	if(last_found==0xFFFF || (l=strlen(search_string))==0
	|| file_length<(l=strlen(search_string))) return;
	if(last_found+1>=file_length) last_found=0; /* one less than 0 */
	  else last_found++;
    last_found=search(last_found,l);
	if(last_found==0xFFFF)
	{
		beep();
        last_found=search(0,l);
	}
	if(last_found==0xFFFF)
		return;
	refresh_off();
	cursor_tof();
	cur_offset=last_found;
	locate_cursor();
	refresh_on();
}

void unerase_line(void)
{
    static byte l;
    register char *s=saved_line;
	cancel();
	cursor_tol();
    l=strlen(saved_line);
    while(*s)
    {
        if(*s==127) *s=0;
        s++;
    }
    insert_from(saved_line,l);
    s=saved_line;
    while(l)
    {
        if(!*s) *s=127;
        s++;
        l--;
    }
}

void _ozhline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,len,1,color);
}

void _ozvline(byte x,byte y,byte len,byte color)
{
	_ozfilledbox(x,y,1,len,color);
}

byte cursor_y;

void do_cursor_on(void)
{
#ifndef FIXED
	static byte x;
#endif /* not FIXED */
	extern byte cursor_byte1,cursor_byte2,cursor_active,cursor_count;
	extern byte cursor_height,cursor_state;
	extern unsigned cursor_pos;
	if(!refresh) return;
#ifndef FIXED
	if(display_x>WIDTH-1) x=WIDTH-2;
	 else if(display_x==0) x=0;
	  else x=display_x-1;
	ozdisableinterrupts();
	cursor_height=line_height;
    cursor_pos=0xA000+cursor_y*30+(x>>3);
#ifdef EXTENDED_CURSOR
    if(cursor_y>0)
	{
		cursor_pos-=30;
		cursor_height++;
	}
    if(line_height<13 && cursor_y+line_height<79) cursor_height++;
#endif
	x&=7;
#ifndef THIN_CURSOR
	if(x<7)
	{
		cursor_byte1=3<<x;
#else /* THIN_CURSOR */
		cursor_byte1=1<<x;
#endif /* THIN_CURSOR */
		cursor_byte2=0;
#ifndef THIN_CURSOR
	}
	else
	{
		cursor_byte1=0x80;
		cursor_byte2=1;
	}
#endif /* THIN_CURSOR */
#else /* FIXED */
	ozdisableinterrupts();
	cursor_height=line_height;
    cursor_pos=0xA000+y*line_height_bytes+(display_x>>3);
	if(display_x==236)
		cursor_byte1=7;
	else
		cursor_byte1=0xF;
	if(display_x&7) cursor_byte1<<=4;
	cursor_byte2=0;
#endif /* FIXED */
	cursor_state=1;
	cursor_active=1;
	cursor_count=FLASH_RATE;
	doxorcursor();
	ozenableinterrupts();
}

void cursor_on(void)
{
    cursor_y=y*line_height;
    do_cursor_on();
}

void cursor_off(void)
{
	extern byte cursor_active,cursor_state;
	if(!cursor_active || !refresh) return;
	ozdisableinterrupts();
	cursor_active=0;
	if(cursor_state)
	{
		cursor_state=0;
		doxorcursor();
	}
	ozenableinterrupts();
}


byte ozputs0(byte y,char *s)
{
    return ozputs(0,y,s);
}

#ifndef FIXED
byte put_x,put_y;

void putcharacter(char c)
{
    put_x=ozputch(put_x,put_y,c);
}

void ozputsseq(byte y,char *s)
{
    register char *p=s;
    put_x=0;
    put_y=y;
    while(*p)
    {
        if(*p=='\n')
        {
            put_y+=8;
            put_x=0;
        }
        else
        {
            if(*p&0x80)
            {
                putcharacter(*p&0x7f);
                put_x+=2;
            }
            else
                putcharacter(*p);
        }
        p++;
    }
}
#endif

void ozcls(void)
{
    cleararea(0,2400);
}

unsigned ozreadauxword2(unsigned k)
{
    return ozreadauxword(2*k);
}

unsigned getch_cursor(void)
{
    register void *k;
    cursor_on();
    k=(void*)getch();
    cursor_off();
    return (unsigned)k;
}

int ozputs(int x,int y,char *s)
{
    static byte r;
    extern unsigned ozputsfx;
    extern byte skipclearret;
    strncpy(line_buf,s,120);
    line_buf[120]=0;
    ozputsfx=x;
    skipclearret=0xC9;
    r=ozputsf(y);
    skipclearret=ozputsfx=0;
    return r;
}

int ozputch(int x,int y,char c)
{
    static char s[2]={ 0,0 };
    s[0]=c;
    return ozputs(x,y,s);
}

