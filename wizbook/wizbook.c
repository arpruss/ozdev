#include <oz.h>
#include <string.h>
#include "wizbook.h"

/* #define OZSNAP /**/

extern unsigned index_offset; /* set up */
byte dorev;
unsigned ver;
unsigned delta;
byte chartablesize;
byte myfont;
byte line_height;
byte font_height;
byte num_lines;
byte backandforth;
byte show_bar;
unsigned scrollbar_pos;
unsigned long thepos;
byte revvideo;

char bookmark_marker[]="BookMrkA";

unsigned long buffer_offset;
unsigned screen_offset;
unsigned in_buf;
byte ateof;
extern byte filebuf[FILEBUF_LEN+1];
unsigned header_length;
unsigned numwords;
unsigned dictionary_length;
byte index_data_length;
unsigned index_length;
extern unsigned dict_idx[MAX_WORDS+1];
unsigned long length;
fname filename;

char error_dict[]="in dict.";
char error_seek[]="seeking.";
char error_read[]="reading.";
char anykey[]="Press any key";
char yn[]="Yes:[ENTER]/No:[ESC]";
char directory[]="wb/";

byte reseek;

unsigned long last;
unsigned long bookmarks[MAX_BOOKMARKS];
unsigned long positions[MAX_POSITIONS];
byte indentnum;
byte positionptr;
byte numpositions;
byte numbookmarks;
byte bookmarkptr;
byte bookmarksmod;
unsigned start_toc;
unsigned num_toc_entries;

#define warnanykey(s) ozwarn(s,anykey)

void myread_check(void *p, int n)
{
    if(read1(p,n)<n) errread();
}

void box239(byte y0,byte h,byte c)
{
    _ozfilledbox(0,y0,239,h,c);
}

unsigned long getbookmark(unsigned n)
{
    return bookmarks[n];
}

void delbookmark(unsigned n)
{
    static unsigned i;
    for(i=n;i<numbookmarks;i++)
      bookmarks[i]=bookmarks[i+1];
    numbookmarks--;
    if(bookmarkptr>n) bookmarkptr--;
}

unsigned long position0=0;

unsigned long gettocentry(unsigned n)
{
    ozreadauxmem0(start_toc+3*n,&position0,3);
    return position0;
}

void clearbookmarks(void)
{
   if(numbookmarks && ozwarn("Clear all bookmarks?",yn)==KEY_LOWER_ENTER
       )
       {
           bookmarksmod=1;
           numbookmarks=0;
       }
}


unsigned mygetch(void)
{
    static unsigned k;
    for(;;)
    {
        switch(k=ozgetch())
        {
            case KEY_MYPROGRAMS:
                exit(0);
            case KEY_MAIN:
            case KEY_CALENDAR:
            case KEY_TELEPHONE:
            case KEY_MEMO:
                ozexitto(k);
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            case KEY_UPPER_ENTER:
            case KEY_LOWER_ENTER:
                return KEY_RETURN;
            case KEY_UPPER_ESC:
                return KEY_LOWER_ESC;
            default:
                return k;
        }
    }
}

void setmyfont(void)
{
    ozsetfont(myfont);
}

void setnormfont(void)
{
    ozsetfont(FONT_PC_NORMAL);
}

void puttextline(byte y)
{
    setmyfont();
    ozputsf(y);
    if(revvideo) _ozfilledbox(0,y,revvideo,font_height,XOR);
    setnormfont();
}

static byte fatalerror=0;

void err(char *msg)
{
    fatalerror=1;
    strcpy(error_msg+6,msg);
    warnanykey(error_msg);
	exit(0);
}

void write11(byte *p)
{
    write1(p,1);
}

void savebookmarks(void)
{
    if(fatalerror || !bookmarksmod || strncmp(directory,filename,3)) return;
    close1(); /* just in case */
    ozputs00("Saving bookmarks...");
    filename[0]='b';
    filename[1]='m';
    if(open1(filename,O_WRONLY)<0) return;
    write1(bookmark_marker,8);
    if(searching) last=saved_last; else last=screen_offset+buffer_offset;
    write1(&last,4);
    write11(&numbookmarks);
    write1(bookmarks,sizeof bookmarks);
    write11(&line_height);
    write11(&show_bar);
    write11(&positionptr);
    write11(&numpositions);
    write1(positions,sizeof positions);
    close1();
}


void setnumlines(void)
{
   if(myfont<FONT_CUSTOM0 && !line_height) line_height=9;
   if(line_height<font_height || line_height>font_height+2)
    line_height=font_height;
   if(line_height==9 && font_height<9) num_lines=9;
    else num_lines=80/line_height;
   if(line_height>40)
   {
     warnanykey("Font too big.");
     exit(0);
   }
}

byte x;

void status(char *s) { x=ozputs(x,0,s); }



void load_headers(void)
{
	static unsigned i,n;
    static byte j;
    static byte c;
    startloadheaders();
    status("[setup]");
    for(n=i=0;i<numwords;i++)
	{
        if(n>=dictionary_length) err(error_dict);
		j=0;
		while(!(0x80 & (c=ozreadauxbyte(n+j))) && j<MAX_WORD_LENGTH) j++;
        if(j==(byte)MAX_WORD_LENGTH) err(error_dict);
		ozwriteauxbyte(n+j,c&0x7f);
		j++;
		dict_idx[i]=n;
		n+=j;
	}
    for(;i<MAX_WORDS+1;i++) dict_idx[i]=n;
    if(n<dictionary_length)
    {
        start_toc=n;
        num_toc_entries=(dictionary_length-n)/3;
    }
	buffer_offset=header_length;
    status("[file]");
    reseek=1;
}

void myseek(unsigned long offset)
{
    if(offset!=lseek1set(offset)) err(error_seek);
}

#define shift(delta) { screen_offset-=delta; buffer_offset+=delta; }

void shiftdown(void)
{
	/* shift down by FILEBUF_LEN/2, adjusting screen_offset and buffer_offset */
	/* assume that not at eof and so buffer is full */
	static int r;
	if(reseek)
	{
        myseek(buffer_offset+in_buf);
		reseek=0;
	}
	memcpy(filebuf,filebuf+FILEBUF_LEN/2,FILEBUF_LEN/2);
	r=read1(filebuf+FILEBUF_LEN/2,FILEBUF_LEN/2);
	if(r<FILEBUF_LEN/2)
	{
		ateof=1;
		if(r<0) r=0;
	}
	else ateof=0;
	in_buf=FILEBUF_LEN/2 + r;
    shift(FILEBUF_LEN/2);
	filebuf[in_buf]=0;
    if(buffer_offset+in_buf>=length) ateof=1;
}

void shiftup(void)
{
	/* shift up by FILEBUF_LEN/2, adjusting screen_offset and buffer_offset */
	/* assume that not at tof */
    memcpy(filebuf+FILEBUF_LEN/2,filebuf,FILEBUF_LEN/2);
    if(in_buf>=FILEBUF_LEN/2) in_buf=FILEBUF_LEN;
		else in_buf+=FILEBUF_LEN/2;
    shift(-FILEBUF_LEN/2);
    myseek(buffer_offset);
    myread_check(filebuf,FILEBUF_LEN/2);
	reseek=1;
	filebuf[in_buf]=0;
    if(buffer_offset+in_buf>=length) ateof=1;
      else ateof=0; /* can this ever happen? */
}

static byte uppercase,trimat;

byte *dcompline(byte *pos)
{
	static byte *p;
	register char *out=line_buf;
    byte len;
    static unsigned s0;
	static byte c;
    static byte afterword,notafterword;
    static unsigned num;
    p=pos;
    notafterword=uppercase=afterword=0;
    trimat=254;
	c=*p++;
    if(c==1)
	{
		static byte i;
        if(index_length) revvideo=*p++;
        else revvideo=0;
        for(i=0;i<indentnum;i++) *out++=' ';
	}
	afterword=0;
	while((c=*p) && c!=1)
	{
        extern byte safety;
        if(safety!=(byte)233) err("in file.");
    NEXT:
		p++;

		switch(c)
		{
			case 2:
				*out++=',';
                *out++=' '; afterword=0;
				break;
			case 3:
				*out++='.';
                *out++=' '; afterword=0;
				break;
			case 127:
				*out++='?';
                *out++=' '; afterword=0;
				break;
            case 128:
                uppercase=1;
                c=*p;
                goto NEXT;
            case 129:
                trimat=*p++;
                notafterword=1;
                c=*p;
                goto NEXT;
			case 30:
                num=(NUM_SINGLES-2);
                GET_FROM_DICT:
                num+=(*p++)+delta;
                GET_FROM_DICT0:
				if(afterword) *out++=' ';
                len=dict_idx[num+1]-(s0=dict_idx[num]);
                if(trimat<len) len=trimat;
                ozreadauxmem0(s0,out,len);
                if(uppercase)
                {
                    *out&=(byte)(0xff-0x20);
                    uppercase=0;
                }
                trimat=254;
                out+=len;
                if(notafterword)
                {
                    notafterword=afterword=0;
                }
                else
                    afterword=1;
				break;
			case 31:
                num=(NUM_SINGLES+254-2);
                goto GET_FROM_DICT;
            case 130:
                if(ver)
                {
                    num=(NUM_SINGLES+254*2-2);
                    goto GET_FROM_DICT;
                }
                else goto DEFAULT;
            case 131:
                if(ver)
                {
                    num=(NUM_SINGLES+254*3-2);
                    goto GET_FROM_DICT;
                }
                else goto DEFAULT;
            case 132:
                if(ver)
                {
                    num=(NUM_SINGLES+254*4-2);
                    goto GET_FROM_DICT;
                }
                else goto DEFAULT;
            case 133:
                if(ver)
                {
                    num=(NUM_SINGLES+254*5-2);
                    goto GET_FROM_DICT;
                }
/*                else goto DEFAULT; */
            default:
            DEFAULT:
                if(c>=130)
				{
                    num=c-130-ver;
                    goto GET_FROM_DICT0;
                }
				else
				if(c<32)
				{
					if(afterword) *out++=' ';
					*out++=c-4+'a';
                    *out++=' '; afterword=0;
				}
				else
				{
                    if(afterword && ( isalpha(c)
                    || ext_compare==c /* && isalpha(*p) */ ))
                    {
/*
                        if(ext_compare==c && !isalpha(*p))
                        {
                            ozcls();
                            ozputch(0,0,*p);
                            ozgetch();
                        } */
                        *out++=' ';
                    }
					*out++=c;
					afterword=0;
				}
				break;
		 }
            
	}
	*out=0;
	return p;
}

static byte showbot=0;

void clearrest(byte y)
{
    if(y<80) box239(y,80-y,0);
}

void show(void)
{
	register byte *pos;
    static byte y;
    static byte i;
	pos=filebuf+screen_offset;
    for(y=i=0;i<num_lines;i++,y+=line_height)
	{
		if(!ateof && pos>=filebuf+FILEBUF_LEN-MAX_CLINE)
		{
			shiftdown();
			pos-=FILEBUF_LEN/2;
/*            putformatted(0,"| |",pos-filebuf,FILEBUF_LEN-MAX_CLINE);
            ozgetch(); */
		}
		if(pos>=filebuf+in_buf) break;
        pos=dcompline(pos);
/*        ozputs0(y,line_buf); */
        if(!showbot || i==num_lines-1)
            puttextline(y);
    }
    clearrest(y);
    showbot=0;
}

byte backline(void)
{
    if(screen_offset==0) return 0;
	while(filebuf[--screen_offset]>1);
    return 1;
}

byte forwardline(void)
{
	static unsigned old;
    if(screen_offset>=in_buf) return 0;
	old=screen_offset;
	while(filebuf[++screen_offset]>1);
    if(screen_offset>=in_buf)
    {
        screen_offset=old;
        return 0;
    }
    return 1;
}

void checkshiftup(void)
{
  if(screen_offset<MAX_CLINE && buffer_offset>header_length)
	shiftup();
}

void checkshiftdown(void)
{
	if(screen_offset>=in_buf-MAX_CLINE && !ateof)
		shiftdown();
}

unsigned long trim(long pos)
{
   if(pos>=length) return length-1;
   if(pos<header_length) return header_length;
   return pos;
}

void goto_pos(unsigned long _pos)
{
	static unsigned long pos;
    pos=trim(_pos);
    if(pos<buffer_offset || pos>=buffer_offset+in_buf)
    {
        buffer_offset=(pos-header_length)/FILEBUF_LEN*FILEBUF_LEN + header_length;
        myseek(buffer_offset);
        if(buffer_offset+FILEBUF_LEN<=length)
        {
            in_buf=FILEBUF_LEN;
            if(buffer_offset+in_buf!=length) ateof=0; else ateof=1;
        }
        else
        {
            in_buf=length-buffer_offset;
            ateof=1;
        }
        myread_check(filebuf,in_buf);
        /* if(read1(filebuf,in_buf)<in_buf) errread(); */
        if(buffer_offset+in_buf>=length) ateof=1;
        filebuf[in_buf]=0;
        reseek=0;
    }
	screen_offset=pos-buffer_offset+1;
	checkshiftup();
	backline();
}

byte atstart;

byte position_save(void)
{
    static unsigned long pos;
    pos=screen_offset+buffer_offset;
    if(!atstart)
    {
        bookmarksmod=1;
    }
    else atstart=0;
    if(positionptr && positions[positionptr-1]==pos) return 0;
    if(positionptr>=MAX_POSITIONS)
    {
        for(positionptr=0;positionptr<MAX_POSITIONS-1;positionptr++)
        {
            positions[positionptr]=positions[positionptr+1];
        }
/*        positionptr=MAX_POSITIONS-1; */
    }
    positions[positionptr]=pos;
    numpositions=++positionptr;
    return 1;
}

void tenpercent(int sign)
{
    goto_pos(trim(screen_offset+buffer_offset + sign*(long)(length-header_length)/10));
}


void scrollbar(void)
{
    scrollbar_pos=(0xA000+29)+30*ybar(78);
    /* ((screen_offset+buffer_offset-header_length)*78/(length-header_length)); */
    if(show_bar) bar();
}

static byte mod;

void modified_screen(void)
{
    ozcls();
    mod=1;
}

int prevtocentry(unsigned long pos)
{
    static int ii;
    for(ii=0;ii<num_toc_entries && gettocentry(ii)<=pos;ii++);
    return ii-1;
}

void view(void)
{
    static unsigned k;
    static unsigned long delta;
    static byte i;
    static int j;
    static byte scrolllen;
    static unsigned lh30;
	load_headers();
    in_buf=0;
    ateof=0;
    atstart=1;
    scrollbar_pos=0xa000+29;
    if(last==0) last=header_length;
    goto_pos(last);
	mod=1;
    ozcls();
    atexit(savebookmarks);
    for(;;)
	{
        scrolllen=(_ozprogoptions & (byte)OZ_OPTION_SCROLL_FULL)?num_lines:num_lines-1;
        if(mod)
        {
            if(!backandforth)position_save();
            show();
            unbar();
            scrollbar();
            backandforth=mod=0;
        }
        thepos=screen_offset+buffer_offset;
        lh30=line_height*30;
        switch(k=mygetch())
		{
            case KEY_LEFT:
            case KEY_BACKSPACE:
                if(positionptr<=1) break;
                goto_pos(positions[(--positionptr)-1]);
                if(!positionptr) positionptr=1;
                backandforth=mod=1;
                break;
            case KEY_RIGHT:
                if(positionptr>=numpositions) break;
                goto_pos(positions[positionptr]);
                positionptr++;
                backandforth=mod=1;
                break;
            case 'm':
                /* mark position */
                if(numbookmarks>=MAX_BOOKMARKS)
                {
                    for(i=0;i<MAX_BOOKMARKS-1;i++)
                    {
                        bookmarks[i]=bookmarks[i+1];
                    }
                    numbookmarks--;
                }
                bookmarkptr=numbookmarks;
                bookmarks[numbookmarks++]=thepos;
                bookmarksmod=1;
                break;
            case 'j':
                do_goto();
                modified_screen();
                break;
            case 'b':
                if(numbookmarks)
                {
                    goto_pos(bookmarks[bookmarkptr]);
                    if(bookmarkptr) bookmarkptr--;
                     else bookmarkptr=numbookmarks-1;
                    mod=1;
                }
                break;
            case 'r':
                show_bar=!show_bar;
                bookmarksmod=1;
                unbar();
                scrollbar();
                break;
            case 'h':
            case KEY_UPPER_MENU:
            case KEY_LOWER_MENU:
                dohelp();
                modified_screen();
                break;
            case 't':
                if(!num_toc_entries) break;
                j=prevtocentry(thepos);
                if(j<0) j=0;
                toc(gettocentry,NULL,j,num_toc_entries);
                modified_screen();
                break;
            case 'v':
            VIEW_BM:
                bookmarkptr=toc(getbookmark,delbookmark,bookmarkptr,numbookmarks);
                modified_screen();
                break;
            case 'i':
                box239(0,9,0);
                delta=thepos-header_length;
                setnormfont();
                putformatted(0,"$ : ^/^=|%",filename,
                    delta,length-header_length,
                    (unsigned int)(delta*100/(length-header_length)) );
                box239(0,9,XOR);
                mygetch();
                modified_screen();
                break;
            case 'c':
                clearbookmarks();
                modified_screen();
                break;
            case 'p':
                j=prevtocentry(thepos-1);
                goto_pos(j<0 ? 0 : gettocentry(j));
                mod=1;
                break;
            case 'n':
                j=prevtocentry(thepos);
                goto_pos((++j)==num_toc_entries ? length : gettocentry(j));
                mod=1;
                break;
            case 'a':
            case '-':
			case KEY_PAGEUP:
                for(i=scrolllen;i;i--)
				{
					checkshiftup();
					backline();
				}
				mod=1;
				break;
            case 's':
#ifndef OZSNAP
                dosearch_from(header_length);
                modified_screen();
                break;
#else
                ozsnap(); ozsnap();
                break;
#endif
            case 'g':
                searchagain();
                modified_screen();
                break;
            case 'l':
                line_height++;
                setnumlines();
                bookmarksmod=1;
                modified_screen();
                break;
			case KEY_PAGEDOWN:
            case KEY_RETURN:
            case 'z':
            case ' ':
                for(i=scrolllen;i;i--)
				{
					checkshiftdown();
					forwardline();
				}
				mod=1;
				break;
            case KEY_UP:
                checkshiftup();
                if(!backline()) break;
                unbar();
                ozscrolldown(lh30);
                clearrest(line_height*num_lines);
                dcompline(filebuf+screen_offset);
                puttextline(0);
                position_save();
                scrollbar();
                bookmarksmod=1;
                break;
            case KEY_DOWN:
                checkshiftdown();
                if(forwardline())
                {
                    bookmarksmod=mod=1;
                    showbot=1;
                    unbar();
                    ozscroll(lh30);
                }
                break;
            case KEY_LEFT_SHIFT | MASKSHIFT:
            case KEY_RIGHT_SHIFT | MASKSHIFT:
                switch(mygetch() & 0xF0FF)
                {
                    case '-':
                        ozclick(!ozgetclick());
                        ozsavekeysettings();
                        break;
                    case KEY_PAGEUP:
                    case KEY_UP:
                        tenpercent(-1);
                        mod=1;
                        break;
                    case KEY_PAGEDOWN:
                    case KEY_DOWN:
                        tenpercent(1);
                        mod=1;
                        break;
                    case 's':
                        searchtobookmarksfrom(header_length);
                        goto VIEW_BM;
                    case 'd':
                        searchtobookmarksfrom(thepos);
                        goto VIEW_BM;

                }
                break;
            case KEY_UP | MASKSHIFT:
            case KEY_PAGEUP | MASKSHIFT:
                tenpercent(-1);
                mod=1;
                break;
            case KEY_PAGEDOWN | MASKSHIFT:
            case KEY_DOWN | MASKSHIFT:
                tenpercent(1);
                mod=1;
                break;
			case KEY_2ND:
                switch(mygetch() & 0xF0FF )
				{
                    case '-':
                        ozclick(!ozgetclick());
                        ozsavekeysettings();
                        break;
                    case KEY_PAGEUP:
					case KEY_UP:
                        goto_pos(header_length);
                        mod=1;
						break;
					case KEY_PAGEDOWN:
					case KEY_DOWN:
                        goto_pos(length);
                        mod=1;
						break;
				}
				break;
            case 'd':
                dosearch_from(thepos);
                modified_screen();
                break;
            case 'f':
                _ozprogoptions ^= OZ_OPTION_SCROLL_FULL;
                ozsavekeysettings();
                break;
			case KEY_LOWER_ESC:
                exit(0);
		}
	}
}



void main(void)
{
    start();
    view();
    return;
}

