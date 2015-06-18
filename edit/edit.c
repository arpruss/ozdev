#include <oz.h>
#include "edit.h"

#ifdef SECURE
extern byte wrote;
#endif

/* #define OZSNAP /* */
#define DONATION_INFO /* */

#ifdef TRANSL
extern byte xlat(unsigned k);
#endif
extern byte do_grab_transl;

extern byte _ozclick_setting;

void clicktoggle(void)
{
    _ozclick_setting=!_ozclick_setting;
    ozsavekeysettings();
}

typedef void (*movefun)(void);
void select_move(movefun move);
void unerase_line(void);
unsigned last_found=0xffff;
char yn_message[]="Yes:[ENTER]/No:[ESC]";
char anykey_message[]="Press any key.";
byte modified0=0;

byte modified=0;

byte selection_on;
unsigned selection_anchor;
unsigned selection_start;
unsigned selection_end;
unsigned in_clipboard;
/* char saved_line[LINE_BUF_LEN]; */

#define CURLINE_AT 2
static unsigned old_lines[4];
static unsigned new_lines[4];
static int lines_back;

unsigned cur_offset;
unsigned cur_line_offset;
unsigned screen_offset;
byte ideal_x,display_x,y;
byte font;

byte clip_check(void);

void idealdisplay0(void)
{
    ideal_x=display_x=0;
}

void ifseldel(void)
{
    if(selection_on) delete_selection();
}

void all_changed(void)
{
    static byte i;
    for(i=0;i<lines_per_screen;i++) changed[i]=1;
}

static byte isspace0(char c)
{
    switch(c&0x7f)
	{
		case ' ':
        case 13:
			return 1;
		default:
			return 0;
	}
}

void cursor_tof()
{
    screen_offset=cur_line_offset=cur_offset=y=0;
    idealdisplay0();
	if(refresh)
	{
		show_screen(0);
	}
    else all_changed();
}

void wrap_to(unsigned _to)
{
    static unsigned to;
    to=_to;
    while(to>=wrapped_length && file_length>wrapped_length)
      wrapped_length=wrap_line(wrapped_length);
}

unsigned show_line_r(byte line,unsigned _off)
{
    register char *__off;
    __off=(char*)_off;
    wrap_to((unsigned)__off);
    if(refresh) return show_line(line,(unsigned)__off);
	  else
	  {
		changed[line]=1;
        return find_next_line((unsigned)__off);
	  }
}

void approximate_cursor(void)
{
	static byte total,i;
    static byte lb;
    wrap_to(cur_line_offset);
/*    if(cur_line_offset>=wrapped_length)
        wrapped_length=wrap_line(cur_line_offset); */
	grab_line(cur_line_offset);
	total=0;
    for(i=0;lb=line_buf[i];i++)
	{
		if(total>=ideal_x)
		{
			display_x=total;
			break;
		}
        total+=width(lb);
	}
    if(!lb)
	{
	   if(line_buf[i-1]==13)
	   {
		  i--;
          display_x=total-width(13);
	   }
	   else display_x=total;
	}
	cur_offset=cur_line_offset+i;
}

void locate_cursor(void)
{
    static unsigned offset,prev_offset,first_overflowed;
    static unsigned yy;
	static byte i;
	static unsigned i2;
	register char *p;
	offset=screen_offset;
	if(cur_offset<offset) return;
/*    while(cur_offset>wrapped_length || (cur_offset<file_length &&
	cur_offset>=wrapped_length))
		wrapped_length=wrap_line(wrapped_length);
*/
    wrap_to(cur_offset);
    yy=0;
	prev_offset=offset;
	while(offset <= cur_offset && offset < file_length)
	{
		prev_offset=offset;
		offset=find_next_line(offset);
        if(yy==lines_per_screen) first_overflowed=prev_offset;
        yy++;
	}
    yy--;
    if(yy>=lines_per_screen)
	{
		static unsigned scroll_by;
        scrollup_multiple(scroll_by=yy+1-lines_per_screen);
		for(i2=0;i2<scroll_by;i2++)
			screen_offset=find_next_line(screen_offset);
		if(scroll_by>lines_per_screen)
		{
			for(i2=0;i2<scroll_by-lines_per_screen;i2++)
			  first_overflowed=find_next_line(first_overflowed);
            yy=0;
		}
		else
            yy=lines_per_screen-scroll_by;
        while(yy<lines_per_screen)
		{
            show_line_r(yy,first_overflowed);
			first_overflowed=find_next_line(first_overflowed);
            yy++;
		}
        yy--;
	}
    y=yy;
	cur_line_offset=prev_offset;
	if(cur_line_offset==cur_offset)
	{
        idealdisplay0();
		return;
	}
	grab_line(cur_line_offset);
	p=line_buf;
	ideal_x=0;
	for(i=0;i<cur_offset-cur_line_offset;p++,i++)
      ideal_x+=width(*p);
	if(cur_offset==file_length && cur_offset>cur_line_offset &&
	   ozreadauxbyte(file_length-1)==(0x80 | 13))
	{
        y++;
        idealdisplay0();
        if(y==lines_per_screen)
		{
            y--;
			screen_offset=find_next_line(screen_offset);
			scrollup();
		}
		cur_line_offset=file_length;
	}
	else display_x=ideal_x;
}

void cursor_tol(void)
{
	cur_offset=cur_line_offset;
    idealdisplay0();
}


void cursor_eol(void)
{
	static unsigned i;
	static byte b;
	ideal_x=0;
	for(i=cur_line_offset;i<=file_length;i++)
	{
		if((b=ozreadauxbyte(i)) & 0x80) break;
        ideal_x+=width(b);
	}
	cur_offset=i;
    if(b!=(byte)(0x80|13))
	{
		cur_offset++;
        ideal_x+=width(b&0x7f);
	}
	display_x=ideal_x;
}

unsigned wrap_find_next_line(unsigned l)
{
    wrap_to(l);
/*    if(l>wrapped_length) */
    return find_next_line(l);
/*    else
        return wrapped_length=wrap_line(l); */
}

void cursor_eof(void)
{
	static unsigned next_bottom;
	static int i;
	if(cur_line_offset>=file_length) return;
	for(i=y+1;i<(int)lines_per_screen;i++)
	{
		cur_line_offset=find_next_line(cur_line_offset);
		if(cur_line_offset>=file_length)
		{
			cur_offset=file_length;
			locate_cursor();
			return;
		}
	}
	/* now, bottom_line is the bottom line on the screen */
	while((next_bottom=wrap_find_next_line(cur_line_offset))<file_length)
	{
		screen_offset=find_next_line(screen_offset);
		cur_line_offset=next_bottom;
	}
	if(ozreadauxbyte(file_length-1)==(0x80|13))
	{
		cur_line_offset=file_length;
		screen_offset=find_next_line(screen_offset);
	}
	cur_offset=file_length;
    if(refresh)
      show_screen(screen_offset);
    else all_changed();
    locate_cursor();
}

void save_lines(int lookback)
{
	static int i;
	old_lines[CURLINE_AT]=cur_line_offset;
	if(cur_line_offset<file_length)
		old_lines[CURLINE_AT+1]=find_next_line(cur_line_offset);
	for(i=1;i<=lookback && old_lines[CURLINE_AT+1-i];i++)
		old_lines[CURLINE_AT-i]=find_prev_line(old_lines[CURLINE_AT-i+1]);
	lines_back=i-1;
}

void adjust_lines(int delta, byte force_redraw)
{
	static unsigned offset;
	static int i;
	static int redraw_back;
	if(!file_length)
	{
		ozcls();
        idealdisplay0();
		y=0;
		screen_offset=0;
		wrapped_length=0;
		return;
	}
	offset=old_lines[CURLINE_AT-lines_back];
	clear_wrap_to_end(offset);
	for(i=-lines_back;i<(int)(lines_per_screen-y) && offset<file_length;i++)
	{
		if(i<=1) new_lines[CURLINE_AT+i]=offset;
		offset=wrap_line(offset);
	}
	for(;i<(int)(lines_per_screen-y) && i<=1;i++)
	  new_lines[CURLINE_AT+i]=file_length;
	wrapped_length=offset;
	if(lines_back==2 && (new_lines[CURLINE_AT-1]!=old_lines[CURLINE_AT-1]
		  || cur_offset<new_lines[CURLINE_AT-1]
		  || cur_offset<=new_lines[CURLINE_AT-2]
		  || new_lines[CURLINE_AT-1]==file_length))
		redraw_back=2;
	else if(lines_back && (new_lines[CURLINE_AT]!=old_lines[CURLINE_AT]
		  || cur_offset<new_lines[CURLINE_AT]
		  || cur_offset<=new_lines[CURLINE_AT-1]
		  || new_lines[CURLINE_AT]==file_length))
		redraw_back=1;
	else
		redraw_back=0;
	if(y<redraw_back)
	{
		scrolldown_multiple(redraw_back-y);
		y+=(redraw_back-y);
	}
	for(i=-redraw_back;i<0;i++)
	{
		if(y+i==0) screen_offset=new_lines[CURLINE_AT+i];
		if(new_lines[CURLINE_AT+i]==file_length)
		{
			clear_rest(y+i);
			locate_cursor();
			return;
		}
		show_line_r(y+i,new_lines[CURLINE_AT+i]);
	}
	if(new_lines[CURLINE_AT]<file_length)
	{
		offset=new_lines[CURLINE_AT];
		offset=show_line_r(y,offset);
		if(new_lines[CURLINE_AT+1]<file_length)
		{
			if(new_lines[CURLINE_AT+1]-delta!=old_lines[CURLINE_AT+1]
			 || force_redraw)
			{
				offset=new_lines[CURLINE_AT+0];
				for(i=y;i<lines_per_screen && offset<file_length;i++)
					offset=show_line_r(i,offset);
				clear_rest(i);
			}
		}
		else clear_rest(y+1);
	}
	else clear_rest(y);
	locate_cursor();
}

void cursor_right(void)
{
	static byte p,c2;
	if(cur_offset>=file_length) return;
	p=ozreadauxbyte(cur_offset);
	if(
		 (p&0x80) && p!=(' '|0x80)
		 && ( cur_offset+1 < file_length || p==(13|0x80) )
	 || ( c2 = (cur_offset>cur_line_offset &&
	  (0x80&ozreadauxbyte(cur_offset-1)) ))
	  )
	{
		y++;
        cur_line_offset=find_next_line(cur_line_offset);
		if(y>=lines_per_screen)
		{
			y--;
			scrollup();
			show_line_r(y,cur_line_offset);
			screen_offset=find_next_line(screen_offset);
		}
		if(c2)
            display_x=ideal_x=width(ozreadauxbyte(cur_offset)&0x7f);
		else
            idealdisplay0();
		cur_offset++;
	}
	else
	{
        display_x+=width(ozreadauxbyte(cur_offset) & 0x7f);
		ideal_x=display_x;
		cur_offset++;
	}
}

void cursor_down(void)
{
	static unsigned next;
	if(cur_offset>=file_length) return;
	if((next=find_next_line(cur_line_offset))>=file_length)
	{
		cursor_eol();
		cursor_right(); /* try to push forward */
		return;
	}
	y++;
	cur_line_offset=next;
	approximate_cursor();
	if(y>=lines_per_screen)
	{
		y--;
		scrollup();
		show_line_r(y,cur_line_offset);
		screen_offset=find_next_line(screen_offset);
	}
}

void cursor_up(void)
{
	if(!cur_offset) return;
	if(cur_line_offset==0)
	{
		cursor_tol();
		return;
	}
	cur_line_offset=find_prev_line(cur_line_offset);
	approximate_cursor();
	if(y==0)
	{
		scrolldown();
		show_line_r(0,cur_line_offset);
		screen_offset=cur_line_offset;
	}
	else y--;
}

void cursor_left(void)
{
	static unsigned old_cur_offset;
	if(!cur_offset) return;
	if(cur_offset==cur_line_offset)
	{
		old_cur_offset=cur_offset;
		cursor_up();
		cursor_eol();
		if(old_cur_offset>cur_offset) return;
	}
	cur_offset--;
    display_x-=width(ozreadauxbyte(cur_offset) & 0x7F);
	ideal_x=display_x;
}


void insert_char(char c)
{
	if(file_length>=MAX_FILE_LENGTH) return;
	save_lines(2);
	insert(cur_offset,1);
	ozwriteauxbyte(cur_offset,c);
	file_length++;
	wrapped_length++;
	cur_offset++;
	adjust_lines(1,0);
}

void delete_char_left(void)
{
	if(selection_on)
	{
		delete_selection();
		return;
	}
	if(!cur_offset) return;
	save_lines(2);
	cur_offset--;
	delete(cur_offset,1);
	file_length--;
	wrapped_length--;
	adjust_lines(-1,0);
	modified=1;
}

void delete_line(void)
{
	static int del_size;
	if(cur_line_offset>=file_length) return;
	save_lines(2);
    do_grab_transl=0;
    del_size=grab_line(cur_line_offset)-cur_line_offset;
    do_grab_transl=32;
    strcpy(saved_line,line_buf);
    delete(cur_line_offset,del_size);
	cur_offset=cur_line_offset;
	file_length-=del_size;
	wrapped_length-=del_size;
    adjust_lines(-del_size,1);
}

void cursor_word_left(void)
{
	if(!cur_offset) return;

	do
		cursor_left();
	while(cur_offset && isspace0(ozreadauxbyte(cur_offset)));

	while(cur_offset && !isspace0(ozreadauxbyte(cur_offset-1)))
		cursor_left();
}

void delete_word_left(void)
{
    byte c;
	if(!cur_offset) return;
	refresh_off();
	do
    {
        c=ozreadauxbyte(cur_offset-1);
		delete_char_left();
    }
    while(cur_offset && isspace0(c));  /* keep on deleting spaces */

	while(cur_offset && !isspace0(ozreadauxbyte(cur_offset-1)))
		delete_char_left();
	refresh_on();
}

void cursor_word_right(void)
{
	if(cur_offset>=file_length) return;

	while(cur_offset<file_length && !isspace0(ozreadauxbyte(cur_offset)))
		cursor_right();
	while(cur_offset<file_length && isspace0(ozreadauxbyte(cur_offset)))
		cursor_right();
}

void delete_word_right(void)
{
	static unsigned start_offset;

	if(cur_offset>=file_length) return;

	refresh_off();
	start_offset=cur_offset;
	cursor_word_right();
	while(cur_offset>start_offset)
		delete_char_left();
	refresh_on();
}

#ifndef SECURE
#ifdef FIXED
void ttyputstring(char *_s)
{
    register char *s=_s;
    static char s2[2];
    extern byte currow,curcol;
    s2[1]=0;
    while(*s)
    {
        if(*s=='\n')
        {
            curcol=0;
            currow++;
        }
        else
        {
            s2[0]=(*s)&0x7f;
            ttyprintstring(s2);
            if((*s)&0x80) curcol++;
        }
        s++;
    }
}
#endif

void donation_info(void)
{
#ifdef DONATION_INFO
ozcls();
#ifdef FIXED
curcol=currow=0;
ttyputstring(
#else
ozputsseq(0,
#endif
"(Pres\xF3""MEN\xD5""fo\xF2""hel\xF0""i\xEE""editor.)\n"
"I\xE6""yo\xF5""lik\xE5""MemoEdit\xAC""i\xF4""i\xF3""suggeste\xE4""bu\xF4""no\xF4""required\n"
"yo\xF5""giv\xE5""$\xB8""t\xEF""\xE1""pro-lif\xE5""organizatio\xEE""suc\xE8""a\xF3""Birthright.\n"
"Birthrigh\xF4""i\xF3""\xE1""non-denominationa\xEC""crisi\xF3""pregnancy\n"
"cente\xF2""tha\xF4""yo\xF5""migh\xF4""fin\xE4""i\xEE""you\xF2""phon\xE5""boo\xEB""o\xF2""yo\xF5""can\n"
"jus\xF4""sen\xE4""\xE1""chec\xEB""to:\n"
" BIRTHRIGH\xD4""O\xC6""PITTSBURGH\xAC""INC.\n"
" 20\xB1""Sout\xE8""Crai\xE7""Street\n"
" Pittsburgh\xAC""P\xC1""1521\xB3""USA\n"
"(An\xF9""ke\xF9""t\xEF""continue.)\n");
ozgetch();
#endif
}


void help(void)
{
    showhelp();
    if('d'==ozgetch())
	{
        donation_info();
	}
}
#endif /* SECURE */

void redraw(void)
{
	static byte i;
	if(refresh)
	{
	  ozcls();
	  show_screen(screen_offset);
	  if(selection_on) toggleshowselection();
	}
	else for(i=0;i<lines_per_screen;i++) changed[i]=1;
}


void cancel(void)
{
	if(!selection_on) return;
	if(refresh)
		toggleshowselection();
	selection_on=0;
}

static byte lps_delta;

static void getdelta(void)
{
    lps_delta=((_ozprogoptions&(byte)OZ_OPTION_SCROLL_FULL)?lines_per_screen:lines_per_screen-1);
}

void cursor_pageup0(void)
{
	static byte i;
    static byte moved;
/*    for(i=1;i<lines_per_screen;i++) cursor_up(); */
    getdelta();
    moved=0;
    while(y)
    {
        cursor_up();
        moved++;
    }
    for(i=0;i<lps_delta && cur_offset>0;i++)
    {
        if(cur_line_offset) moved++;
        cursor_up();
    }
    while(moved>lps_delta)
    {
        cursor_down();
        moved--;
    }
}

void cursor_pagedown0(void)
{
	static byte i;
    static unsigned old_line_offset;
    static byte moved;
    getdelta();
/*    for(i=getdelta();i<lines_per_screen;i++) cursor_down(); */
    moved=0;
    i=(lps_delta)*2-y;
    if(_ozprogoptions&(byte)OZ_OPTION_SCROLL_FULL) i--;
    while(i-- && cur_offset<file_length)
    {
        old_line_offset=cur_line_offset;
        cursor_down();
        if(old_line_offset!=cur_line_offset) moved++;
    }
    while(moved>lps_delta)
    {
        cursor_up();
        moved--;
    }
}

void cursor_pageup()
{
    refresh_off();
    cursor_pageup0();
    refresh_on();
}

void cursor_pagedown()
{
    refresh_off();
    cursor_pagedown0();
    refresh_on();
}

clearall()
{
    if(wrapped_length) clear_wrap_to_end(0);
    selection_on=wrapped_length=cur_line_offset=cur_offset=ideal_x=display_x=y    =0;
	refresh=1;
	show_screen(0);
}

void ins(unsigned k)
{
#ifdef TRANSL
    if(k>=128) return;
#else
    if((k>=128 || k<32) && k!=13) return;
#endif
    ifseldel();
	insert_char(k);
	modified=1;
}

byte abandon(void)
{
	if(modified)
	{
        if(warn("Really abandon changes?",
            yn_message)!=KEY_LOWER_ENTER)
		{
			redraw();
			return 0;
		}
	}
	return 1;
}

void del_right(void)
{
	modified=1;
    if(selection_on) delete_selection();
	else
	{
		if(cur_offset!=file_length)
		{
			cursor_right();
			delete_char_left();
		}
	}
}

void insertstamp(void)
{
    register char *s;
    s=time_string();
    while(*s) ins(*s++);
}

void edit(void)
{
    static unsigned k;
	ozcls();
	set_edit_font(font);
/*    saved_line[0]=0; */
	clearall();
    installcursor();
    modified=modified0;
    if(last_found!=0xFFFF) locate_found(last_found);
    for(;;)
    {
    k=getch_cursor();
    switch(k)
	{
        case KEY_PAGEUP | MASK2ND | MASKSHIFT:
		case KEY_UP | MASK2ND | MASKSHIFT:
            select_move(cursor_tof);
			break;
        case KEY_PAGEDOWN | MASK2ND | MASKSHIFT:
        case KEY_DOWN | MASK2ND | MASKSHIFT:
            select_move(cursor_eof);
			break;
		case KEY_RIGHT | MASKCTRL | MASKSHIFT:
            select_move(cursor_word_right);
			break;
		case KEY_LEFT | MASKCTRL | MASKSHIFT:
            select_move(cursor_word_left);
			break;
		case KEY_RIGHT | MASK2ND | MASKSHIFT:
			select_move(cursor_eol);
			break;
		case KEY_LEFT | MASK2ND | MASKSHIFT:
			select_move(cursor_tol);
			break;
		case KEY_RIGHT|MASKSHIFT:
			select_move(cursor_right);
			break;
		case KEY_LEFT|MASKSHIFT:
			select_move(cursor_left);
			break;
		case KEY_DOWN|MASKSHIFT:
            select_move(cursor_down);
			break;
		case KEY_PAGEUP|MASKSHIFT:
            select_move(cursor_pageup0);
			break;
		case KEY_PAGEDOWN|MASKSHIFT:
            select_move(cursor_pagedown0);
			break;
		case KEY_UP|MASKSHIFT:
            select_move(cursor_up);
			break;
		case 8|MASKCTRL:
        case MASK2ND|KEY_BACKSPACE:
            cancel();
			delete_word_left();
			break;
		case KEY_LEFT|MASKCTRL:
			cancel();
			cursor_word_left();
			break;
		case KEY_RIGHT|MASKCTRL:
			cancel();
			cursor_word_right();
			break;
		case 't'-'a'+1: /* ctrl-T */
			cancel();
			delete_word_right();
			break;
		case 'y'-'a'+1: /* ctrl-Y */
			cancel();
			modified=1;
			delete_line();
			break;
        case 's'-'a'+1: /* ctrl-S : stamp time */
            insertstamp();
            break;
		case 7: /* ctrl-G */
			del_right();
			break;
        case KEY_BACKSPACE:
			delete_char_left();
			break;
		case KEY_RIGHT | MASK2ND:
			cancel();
			cursor_eol();
			break;
		case KEY_LEFT | MASK2ND:
			cancel();
			cursor_tol();
			break;
		case 'v'|MASK2ND:
			paste_clipboard();
			break;
#ifndef SECURE
        case KEY_UPPER_MENU:
        case KEY_LOWER_MENU:
			help();
			redraw();
			break;
#endif
        case KEY_MYPROGRAMS:
		case KEY_UPPER_ESC:
        case KEY_MEMO:
        case 27:
			if(!abandon()) break;
#ifndef SECURE
			save_clipboard();
			last_found=0xffff;
#endif
			return;
		case KEY_LEFT:
            cancel();
			cursor_left();
			break;
		case KEY_RIGHT:
			cancel();
			cursor_right();
			break;
		case KEY_UP:
			cancel();
			cursor_up();
			break;
		case KEY_DOWN:
			cancel();
			cursor_down();
			break;
		case KEY_PAGEUP:
			cancel();
			cursor_pageup();
			break;
		case KEY_PAGEDOWN:
			cancel();
			cursor_pagedown();
			break;
#ifdef OZSNAP
        case 'i'-'a'+1:
            ozsnap();
            ozsnap();
            break;
#endif
        case 'u'|MASK2ND:
            send();
            break;
        case 'd'|MASK2ND:
            receive();
            break;
        case KEY_UPPER_ENTER:
		case KEY_UPPER_ENTER|MASK2ND:
        case KEY_LOWER_ENTER|MASK2ND:
        case KEY_LOWER_ENTER:
			if(!clip_check()) break;
			cancel();
#ifndef SECURE
            save_clipboard();
			last_found=0xffff;
            if(k&MASK2ND)
            {
                num_recnums=0;
            }
            else
#endif
                if(!modified) return;
            write_file();
#ifndef SECURE
			return;
#else
            if(wrote) return;
            set_edit_font(font);
            clearall(); /* fall-through in case write was canceled */
            break;
#endif
#ifndef FIXED
		case 'f'|MASK2ND:
			font=(font+1)%NUM_FONTS;
            set_edit_font(font);
            ozcls();
			clearall();
			break;
#endif
        case KEY_PAGEUP|MASK2ND:
		case KEY_UP|MASK2ND:
			cancel();
			cursor_tof();
			break;
		case 's'|MASK2ND:
			cancel();
			edit_search();
			break;
		case 'g'|MASK2ND:
			cancel();
			edit_search_again();
			break;
		case 'c'|MASK2ND:
            if(!selection_on) break;
            cancel();
            copy_clipboard();
			break;
		case 'x'|MASK2ND:
			cut_clipboard();
			break;
        case KEY_PAGEDOWN|MASK2ND:
		case KEY_DOWN|MASK2ND:
			cancel();
			cursor_eof();
			break;
        case 'u'-'a'+1:
            unerase_line();
            break;
		case KEY_BACKLIGHT:
			oztogglelight();
			break;
        case '\n':
            ins(13);
            break;
        case 0x1F:
            clicktoggle();
            break;
		default:
#ifdef TRANSL
            ins(xlat(k));
#else
            ins(k);
#endif
			break;
	}
    }
}

