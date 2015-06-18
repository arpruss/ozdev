#include <oz.h>
#include <string.h>
#include <ctype.h>
#include "wizbook.h"
#define FILEBUF_LEN 7750 /* must be even */

byte myfont;
byte line_height;
byte num_lines;
byte backandforth;
byte show_bar;
unsigned scrollbar_pos;

typedef char fname[MAX_FILENAMELEN+1];
fname filename;

char error_header[]="Error in header.";
char error_font[]="Error in font.";
char error_dict[]="Error in compression dictionary.";
char error_seek[]="Error seeking.";
char anykey[]="Press any key";
char yn[]="Yes:[ENTER]/No:[ESC]";
char directory[]="wb/";
extern char line_buf[];

byte reseek;

#define MAX_BOOKMARKS 10 /* per file */
#define MAX_POSITIONS 40
unsigned long last;
unsigned long bookmarks[MAX_BOOKMARKS];
unsigned long positions[MAX_POSITIONS];
byte indentnum;
byte positionptr;
byte numpositions;
byte numbookmarks;
byte bookmarkptr;
byte bookmarksmod;

#define WIDTH 239
#define MAX_WORD_LENGTH 20
#define MAX_WORDS (126+254+254) /* maximum number of words in dictionary */
#define MAX_CLINE 123
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
unsigned dict_idx[MAX_WORDS];
byte dict_lens[MAX_WORDS];
unsigned long length;

void puttextline(byte y)
{
    static byte f;
    f=ozgetfont();
    ozsetfont(myfont);
    ozputsf(y);
    ozsetfont(f);
}

void err(char *msg)
{
	ozwarn(msg,anykey);
	exit(0);
}

void savebookmarks(void)
{
    if(!bookmarksmod || strncmp(directory,filename,3)) return;
    close1(); /* just in case */
    _ozfilledbox(0,0,WIDTH,line_height,0);
    ozputs(0,0,"Saving bookmarks...");
    filename[0]='b';
    filename[1]='m';
    if(open1(filename,O_WRONLY)<0) return;
    write1("BookMrkA",8);
    last=screen_offset+buffer_offset;
    write1(&last,4);
    write1(&numbookmarks,1);
    write1(bookmarks,sizeof bookmarks);
    write1(&line_height,1);
    write1(&show_bar,1);
    write1(&positionptr,1);
    write1(&numpositions,1);
    write1(positions,sizeof positions);
    close1();
}

byte x;

void none(void)
{
    x=ozputs(x,0,"(none)");
}

void load_headers(void)
{
	static char hdr[8];
	static unsigned i,n;
	static byte j,c;
    static struct ozfontheader fheader;
    bookmarksmod=0;
    backandforth=0;
    numbookmarks=last=0;
    show_bar=1;
    ozcls();
    x=0;
    if(!strncmp(directory,filename,3))
    {
        x=ozputs(0,x,"[bookmarks]");
        filename[0]='b';
        filename[1]='m';
        if(open1(filename,O_RDONLY)>=0)
        {
            if(read1(hdr,8)==8
            && !strncmp(hdr,"BookMrkA",8)
            && read1(&last,4)==4
            && read1(&numbookmarks,1)==1
            && read1(bookmarks,sizeof bookmarks)==sizeof bookmarks
            )
            {
                if(numbookmarks) bookmarkptr=numbookmarks-1;
                read1(&line_height,1);
                if(line_height<8 || line_height>10) line_height=8;
                if(line_height!=9) num_lines=80/line_height;
                 else num_lines=9;
                if(read1(&show_bar,1)<1 || show_bar>1)
                {
                    show_bar=1;
                }
                if(read1(&positionptr,1)<1
                    || positionptr>MAX_POSITIONS
                    || read1(&numpositions,1)<1
                    || numpositions>MAX_POSITIONS
                    || read1(positions,sizeof positions)<sizeof positions)
                {
                    numpositions=positionptr=0;
                }
                else backandforth=1;
            }
            else
            {
                numbookmarks=last=0;
                show_bar=1;
                none();
            }
            close1();
        }
        else
        {
            none();
        }
        filename[0]='w';
        filename[1]='b';
    }
    else
    {
        none();
    }
    x=ozputs(x,0,"[header]");
	if(open1(filename,O_RDONLY)<0) err("Cannot open!");
	if(read1(hdr,8)<8
	|| strncmp(hdr,"WizBookA",8)
	|| read1(&header_length,2)<2
	|| read1(&length,4)<4
	|| read1(&numwords,2)<2
	|| read1(&dictionary_length,2)<2
	|| header_length>16384
    || header_length<dictionary_length+10) err(error_header);
	header_length+=10;
    if(last==0) last=header_length;
    x=ozputs(x,0,"[dictionary]");
	for(i=0;i<dictionary_length;i+=n)
	{
		n=dictionary_length-i;
		if(n>FILEBUF_LEN) n=FILEBUF_LEN;
        if(read1(filebuf,n)<n) err(error_dict);
		ozwriteauxmem(i,filebuf,n);
	}
    if(read1(&index_length,1)<1 || read1(&indentnum,1)<1) err(error_header);
    myfont=0xFF;
    if(indentnum&0x80)
        myfont=FONT_CUSTOM0;
    else
    if(indentnum&0x40)
    {
        x=ozputs(x,0,"[font]");
        if(read1(&fheader,sizeof fheader)<sizeof fheader) err(error_font);
        for(i=0;i<sizeof fheader;i++) ozwritecustomfontbyte(0,i,((byte*)&fheader)[i]);
        for(i=sizeof fheader;i<fheader.length;i++)
        {
            if(read1(&c,1)<1) err(error_font);
            ozwritecustomfontbyte(0,i,c);
        }
        myfont=FONT_CUSTOM0;
    }
    if(myfont!=0xFF)
    {
        ozscancustomfonts();
        line_height=ozgetfontheight(myfont);
        num_lines=80/line_height;
    }
    else
    {
        num_lines=line_height=9;
        myfont=FONT_OZ_NORMAL;
    }
    indentnum&=0x3f;
    for(n=i=0;i<numwords;i++)
	{
		j=0;
		while(!(0x80 & (c=ozreadauxbyte(n+j))) && j<MAX_WORD_LENGTH) j++;
        if(j==MAX_WORD_LENGTH) err(error_dict);
		ozwriteauxbyte(n+j,c&0x7f);
		j++;
		dict_idx[i]=n;
		dict_lens[i]=j;
		n+=j;
	}
	buffer_offset=header_length;
    ozputs(x,0,"[file]");
}

void shiftdown(void)
{
	/* shift down by FILEBUF_LEN/2, adjusting screen_offset and buffer_offset */
	/* assume that not at eof and so buffer is full */
	static int r;
    static long r2;
	if(reseek)
	{
        r2=lseek1(buffer_offset+in_buf,SEEK_SET);
        if(r2!=buffer_offset+in_buf) err(error_seek);
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
	screen_offset-=FILEBUF_LEN/2;
	buffer_offset+=FILEBUF_LEN/2;
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
	buffer_offset-=FILEBUF_LEN/2;
	screen_offset+=FILEBUF_LEN/2;
	if(buffer_offset!=lseek1(buffer_offset,SEEK_SET)) err("Error seeking!");
	if(read1(filebuf,FILEBUF_LEN/2) < FILEBUF_LEN/2) err("Error reading!");
	reseek=1;
	filebuf[in_buf]=0;
    if(buffer_offset+in_buf>=length) ateof=1;
      else ateof=0; /* can this ever happen? */
}

static byte uppercase,trimat;

unsigned dumpfromdict(char *out,unsigned n)
{
	static byte len;
    len=dict_lens[n];
    if(trimat<len) len=trimat;
    ozreadauxmem(dict_idx[n],out,len);
    if(uppercase) *out&=(0xff-0x20);
    uppercase=0;
    trimat=254;
	return len;
}

byte *dcompline(byte *pos)
{
	static byte *p;
	register char *out=line_buf;
	static byte c;
    static byte afterword,notafterword;
    p=pos;
    notafterword=uppercase=afterword=0;
    trimat=254;
	c=*p++;
    if(c==1)
	{
		static byte i;
		for(i=0;i<index_length;i++) p++; /* skip index data if any */
        for(i=0;i<indentnum;i++) *out++=' ';
	}
	afterword=0;
	while((c=*p) && c!=1)
	{
    NEXT:
		p++;
		switch(c)
		{
			case 2:
				*out++=',';
				*out++=' ';
				afterword=0;
				break;
			case 3:
				*out++='.';
				*out++=' ';
				afterword=0;
				break;
			case 127:
				*out++='?';
				*out++=' ';
				afterword=0;
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
				if(afterword) *out++=' ';
                out+=dumpfromdict(out,(*p++)-2+126);
                if(notafterword)
                {
                    notafterword=afterword=0;
                }
                else
                    afterword=1;
				break;
			case 31:
				if(afterword) *out++=' ';
                out+=dumpfromdict(out,(*p++)-2+126+254);
                if(notafterword)
                {
                    notafterword=afterword=0;
                }
                else
                    afterword=1;
				break;
			default:
                if(c>=130)
				{
					if(afterword) *out++=' ';
                    out+=dumpfromdict(out,c-130);
                    if(notafterword)
                    {
                        notafterword=afterword=0;
                    }
                    else
                        afterword=1;
				}
				else
				if(c<32)
				{
					if(afterword) *out++=' ';
					*out++=c-4+'a';
					*out++=' ';
					afterword=0;
				}
				else
				{
					if(afterword && isalpha(c)) *out++=' ';
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
/*            putformatted(0,0,"| |",pos-filebuf,FILEBUF_LEN-MAX_CLINE);
            ozgetch(); */
		}
		if(pos>=filebuf+in_buf) break;
        pos=dcompline(pos);
/*		  ozputs(0,y,line_buf); */
        if(!showbot || i==num_lines-1)
            puttextline(y);
    }
	if(y<80) _ozfilledbox(0,y,239,80-y,0);
    showbot=0;
}

int backline(void)
{
/*    ozputch(0,0,'0'+(screen_offset==0));
    ozgetch(); */
    if(screen_offset==0) return 0;
	while(filebuf[--screen_offset]>1);
    return 1;
}

int forwardline(void)
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

void goto_pos(unsigned long _pos)
{
	static unsigned long pos;
	pos=_pos;
    if(pos<buffer_offset || pos>=buffer_offset+in_buf)
    {
        if(pos>=length) pos=length-1;
        if(pos<header_length) pos=header_length;
        buffer_offset=(pos-header_length)/FILEBUF_LEN*FILEBUF_LEN + header_length;
        if(buffer_offset!=lseek1(buffer_offset,SEEK_SET)) err("Error seeking!");
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
        if(read1(filebuf,in_buf)<in_buf) err("Error reading!");
        if(buffer_offset+in_buf>=length) ateof=1;
        filebuf[in_buf]=0;
        reseek=0;
    }
	screen_offset=pos-buffer_offset+1;
	checkshiftup();
	backline();
/*    printf("%ld %ld",pos,screen_offset+buffer_offset); */
}

byte atstart;

byte position_save(void)
{
    static byte i;
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
        for(i=0;i<MAX_POSITIONS-1;i++)
        {
            positions[i]=positions[i+1];
        }
        positionptr=MAX_POSITIONS-1;
    }
    positions[positionptr]=pos;
    numpositions=++positionptr;
    return 1;
}

void tenpercent(int sign)
{
    static long topos;
    topos=screen_offset+buffer_offset + sign*(long)(length-header_length)/10;
    if(topos<(long)header_length) topos=header_length;
    if(topos>length) topos=length;
    goto_pos(topos);
}


void scrollbar(void)
{
    scrollbar_pos=(0xA000+29)+30*(unsigned int)((screen_offset+buffer_offset-header_length)*78/(length-header_length));
    if(show_bar) bar();
}

void view(void)
{
	static byte mod;
	static unsigned k;
    static unsigned long delta;
    static byte i;
	load_headers();
    in_buf=0;
    ateof=0;
    atstart=1;
    scrollbar_pos=0xa000+29;
    if(last==0) last=header_length;
    goto_pos(last);
	mod=1;
    ozcls();
	while(1)
	{
        if(mod)
        {
            if(!backandforth)position_save();
            show();
            unbar();
            scrollbar();
            backandforth=mod=0;
        }
        switch(k=ozgetchblank())
		{
            case KEY_LEFT:
            case KEY_BACKSPACE:
#ifdef DEBUG
                putformatted(0,0,"positionptr=| numpositions=|  ",
                positionptr,numpositions);
                ozgetch();
#endif
                if(positionptr<=1) break;
                goto_pos(positions[(--positionptr)-1]);
                if(!positionptr) positionptr=1;
                backandforth=mod=1;
#ifdef DEBUG
                putformatted(0,0,"positionptr=| numpositions=|  ",
                positionptr,numpositions);
                ozgetch();
#endif
                break;
            case KEY_RIGHT:
#ifdef DEBUG
                putformatted(0,0,"positionptr=| numpositions=|  ",
                positionptr,numpositions);
                ozgetch();
#endif
                if(positionptr>=numpositions) break;
                goto_pos(positions[positionptr]);
                positionptr++;
                backandforth=mod=1;
#ifdef DEBUG
                putformatted(0,0,"positionptr=| numpositions=|  ",
                positionptr,numpositions);
                ozgetch();
#endif
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
                bookmarks[numbookmarks++]=screen_offset+buffer_offset;
                bookmarksmod=1;
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
                ozcls();
                mod=1;
                break;
            case 'i':
                _ozfilledbox(0,0,WIDTH,line_height,0);
                delta=screen_offset+buffer_offset-header_length;
                putformatted(0,0,"$ : ^/^=|%",filename,
                    delta,length-header_length,
                    (unsigned int)(delta*100/(length-header_length)) );
                _ozfilledbox(0,0,WIDTH,line_height,XOR);
                ozgetchblank();
                mod=1;
                ozcls();
                break;
            case 'c':
                if(numbookmarks && ozwarn("Delete all bookmarks?",yn)==KEY_LOWER_ENTER
                    )
                    {
                        bookmarksmod=1;
                        numbookmarks=0;
                    }
                mod=1;
                ozcls();
                break;
            case '-':
			case KEY_PAGEUP:
                for(i=0;i<num_lines-1;i++)
				{
					checkshiftup();
					backline();
				}
				mod=1;
				break;
            case 's':
                if(myfont) break;
                line_height++;
                if(line_height==11) line_height=8;
                if(line_height!=9) num_lines=80/line_height;
                    else num_lines=9;
                bookmarksmod=1;
                mod=1;
                ozcls();
                break;
			case KEY_PAGEDOWN:
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
            case KEY_RETURN:
            case ' ':
                for(i=0;i<num_lines-1;i++)
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
                ozscrolldown(line_height*30);
                dcompline(filebuf+screen_offset);
                puttextline(0);
                position_save();
                scrollbar();
                break;
            case KEY_DOWN:
                checkshiftdown();
                if(forwardline())
                {
                    mod=1;
                    showbot=1;
                    unbar();
                    ozscroll(line_height*30);
                }
                break;
            case KEY_LEFT_SHIFT | MASKSHIFT:
            case KEY_RIGHT_SHIFT | MASKSHIFT:
                switch(ozgetch() & 0xF0FF)
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
                switch(ozgetch() & 0xF0FF )
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
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
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
		}
	}
}

char *ozgetnextfilename0(byte mode)
{
    register char *s;
    while(NULL!=(s=ozgetnextfilename(mode)))
    {
        if(strncmp(s,"bm/",3)) return s;
        mode &= ~FIND_RESET;
    }
    return NULL;
}

void nofiles(void)
    {
        ozcls();
        ozwarn("No files!",anykey);
        exit(0);
    }


char *selectname(void)
{
	static fname files[9];
	static byte refresh;
	static byte pos;
	static unsigned i;
    static unsigned i1;
	static unsigned k;
	static unsigned curpos;
    static unsigned cursorloc;
	register char *f;
    cursorloc=curpos=0;
	ozfont(FONT_OZ_NORMAL);
    f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
    if(f==NULL) nofiles();
	do
	{
        background();
		for(pos=i=0;i<9;i++,pos+=8)
		{
            if(i && NULL==(f=ozgetnextfilename0(FIND_OWNED))) break;
			strcpy(files[i],f);
            putformatted(0,pos,"|. $",i+1,f);
/*
			ozputch(0,pos,i+'1');
			ozputch(5,pos,'.');
			ozputs(9,pos,f);
*/
		}
		curpos+=9;
        if(cursorloc>=i) cursorloc=i-1;
/*        ozputs(0,72,"\x0A/\x0B:choose  \x0D/1-9:load  Del:del.  C:clear bkmrks  A:del. all"); */
        if(NULL!=f) f=ozgetnextfilename0(FIND_OWNED);
		refresh=0;
		do
		{
          _ozfilledbox(0,8*cursorloc,150,8,XOR);
		  k=ozgetchblank();
          _ozfilledbox(0,8*cursorloc,150,8,XOR);
          switch(k)
          {
             case KEY_LOWER_ESC:
             case KEY_UPPER_ESC:
                exit(0);
             case KEY_MAIN:
             case KEY_TELEPHONE:
             case KEY_SCHEDULE:
             case KEY_MEMO:
             case KEY_MYPROGRAMS:
                ozexitto(k);
             case KEY_BACKLIGHT:
                oztogglelight();
                break;
             case 'a':
               if(ozwarn("Are you sure you want to delete all?",yn)!=
                KEY_LOWER_ENTER)
               {
                    refresh=1;
                    f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
                    cursorloc=curpos=0;
                    break;
               }
               ozunlinkallowned();
               exit(0);
             case KEY_BACKSPACE:
             case 'c':
               ozcls();
               f=files[cursorloc];
               ozputs(ozputs(0,0,k==KEY_BACKSPACE?"Deleting: ":"Clearing bookmarks for: "),0,f);
               if(ozwarn("Are you sure?",yn)==KEY_LOWER_ENTER)
               {
                   if(k==KEY_BACKSPACE) unlink(f);
                   if(!strncmp(f,directory,3))
                   {
                     f[0]='b';
                     f[1]='m';
                     unlink(f);
                   }
               }
               refresh=1;
               f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
               if(f==NULL) nofiles();
               cursorloc=curpos=0;
               break;
             case KEY_LOWER_ENTER:
             case KEY_UPPER_ENTER:
             case KEY_RETURN:
                return files[cursorloc];
             case KEY_UP:
                if(cursorloc) cursorloc--;
                else if(curpos>=18)
                {
                    cursorloc=8;
                    k=KEY_PAGEUP;
                }
                break;
             case KEY_DOWN:
                if(cursorloc<i-1) cursorloc++;
                else if(f!=NULL)
                {
                    cursorloc=0;
                    k=KEY_PAGEDOWN;
                }
                break;
          }
          switch(k)
          {
            case KEY_PAGEUP:
                      if(curpos>=18)
                      {
                        curpos-=18;
                        f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
                        for(i1=0;i1<curpos;i1++)
                          ozgetnextfilename0(FIND_OWNED);
                        refresh=1;
                      }
                      else
                        cursorloc=0;
                      break;
            case KEY_PAGEDOWN:
                      refresh=1;
                      if(f==NULL)
                      {
                         cursorloc=i-1;
                         refresh=0;
                      }
                      break;
          }
		  if(k>='1' && k<'1'+i) refresh=1;
		} while(!refresh);
	} while(k<'1' || k>='1'+i);
	return files[k-'1'];
}

void main(void)
{
    extern unsigned _ozautoofftime;
    _ozautoofftime=60*10/4;
    preparehelp();
    ozrestorekeysettings();
    ozcls();
    ozputs(0,0,"Checking for new books...");
    frommemo();
    bookmarksmod=0;
    strcpy(filename,selectname());
    ozcls();
    atexit(savebookmarks);
    view();
    return;
}

