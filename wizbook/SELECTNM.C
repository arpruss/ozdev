#include <oz.h>
#include "wizbook.h"
#include "../hitech/liboz/getfile.c"

extern unsigned index_offset;
extern unsigned index_area_length;
extern char entry_format[];
extern char error_header[];
extern char error_font[];
extern char clear_msg[];
extern char del_msg[];
extern char sure_msg[];
extern char delall_msg[];
extern char nofiles_msg[];
extern char bookmarks_msg[];
extern char header_msg[];
extern char dict_msg[];
extern char wizbook_header[];
extern char font_msg[];
extern char ext_msg[];
extern char bm_dir[];

byte myread(void *s,int n)
{
    return read1(s,n)==n;
}

char *utoa_05(unsigned u)
{
    extern char utoa_buffer[6]; /* zeroed */
    register char *b;
    byte ndigits=5;
    b=utoa_buffer+5;
    while(ndigits--)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    return b;
}

static char *ozgetnextfilename0(byte mode)
{
    register char *s;
    while(NULL!=(s=ozgetnextfilename(mode)))
    {
        if(strncmp(s,bm_dir,3)) return s;
        mode &= ~FIND_RESET;
    }
    return NULL;
}

static void nofiles(void)
    {
        ozcls();
        ozwarn(nofiles_msg,anykey);
        exit(0);
    }

static char *selectname(void)
{
    extern fname files[9];
	static byte refresh;
    static byte pos;
	static unsigned i;
    static unsigned i1;
	static unsigned k;
	static unsigned curpos;
    static unsigned cursorloc;
	register char *f;
    cursorloc=curpos=0;
    setnormfont();
    f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
    if(f==NULL) nofiles();
	do
	{
        background();
		for(pos=i=0;i<9;i++,pos+=8)
		{
            if(i && NULL==(f=ozgetnextfilename0(FIND_OWNED))) break;
			strcpy(files[i],f);
            putformatted(pos,entry_format,i+1,f);
		}
		curpos+=9;
        if(cursorloc>=i) cursorloc=i-1;
        if(NULL!=f) f=ozgetnextfilename0(FIND_OWNED);
		refresh=0;
		do
		{
          _ozfilledbox(0,8*cursorloc,150,8,XOR);
          k=mygetch();
          _ozfilledbox(0,8*cursorloc,150,8,XOR);
          switch(k)
          {
             case KEY_LOWER_ESC:
                exit(0);
             case 'a':
               if(ozwarn(delall_msg,yn)!=
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
             case 'u':
               ozcls();
               f=files[cursorloc];
               ozputs(ozputs00(k!='c'?del_msg:
                    clear_msg),0,f);
               if(ozwarn(sure_msg,yn)==KEY_LOWER_ENTER)
               {
                   if(k!='c') unlink(f);
                   if(!strncmp(f,directory,3) && k!='u')
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

void start(void)
{
    ozsinglepage();
    ozrestorekeysettings();
    frommemo();
    ozsetowner(1005);
    bookmarksmod=0;
    strcpy(filename,selectname());
    ozcls();
}

extern char none[];
extern struct ozfontheader fheader;
extern char hdr[8];

void startloadheaders(void)
{
    static unsigned i,n;
    static byte c;
    bookmarksmod=0;
    backandforth=0;
    numbookmarks=last=0;
    show_bar=1;
    ozcls();
    x=0;
    if(!strncmp(directory,filename,3))
    {
        status(bookmarks_msg);
        filename[0]='b';
        filename[1]='m';
        if(open1(filename,O_RDONLY)>=0)
        {
            if(myread(hdr,8)
            && !strncmp(hdr,bookmark_marker,8)
            && myread(&last,4)
            && myread(&numbookmarks,1)
            && myread(bookmarks,sizeof bookmarks)
            )
            {
                if(numbookmarks) bookmarkptr=numbookmarks-1;
                read1(&line_height,1);
                if(!myread(&show_bar,1) || show_bar>1)
                {
                    show_bar=1;
                }
                if(!myread(&positionptr,1)
                    || positionptr>MAX_POSITIONS
                    || !myread(&numpositions,1)
                    || numpositions>MAX_POSITIONS
                    || !myread(positions,sizeof positions))
                {
                    numpositions=positionptr=0;
                }
                else backandforth=1;
            }
            else
            {
                numbookmarks=last=0;
                show_bar=1;
                status(none);
            }
            close1();
        }
        else
        {
            status(none);
        }
        filename[0]='w';
        filename[1]='b';
    }
    else
    {
        status(none);
    }
    status(header_msg);
    if(open1(filename,O_RDONLY)<0) errread();
    myread_check(hdr,8);
    myread_check(&header_length,2);
    myread_check(&length,4);
    myread_check(&numwords,2);
    myread_check(&dictionary_length,2);
    if(strncmp(hdr,wizbook_header,7)
    || numwords>MAX_WORDS
    || dictionary_length>16384
    || header_length<dictionary_length+10
    || ( ver=4*(hdr[7]-'A') )>4 ) err(error_header);
    delta=4-ver;
	header_length+=10;
    if(last==0) last=header_length;
    status(dict_msg);
/*    putformatted(0,10,"| | |",header_length,dictionary_length,numwords);
    ozgetch(); */
    for(i=0;i<dictionary_length;i+=n)
	{
		n=dictionary_length-i;
		if(n>FILEBUF_LEN) n=FILEBUF_LEN;
        myread_check(filebuf,n);
        ozwriteauxmem0(i,filebuf,n);
	}
    myread_check(&index_length,1);
    myread_check(&indentnum,1);
    myfont=0xFF;
    if(indentnum&0x80)
        myfont=FONT_CUSTOM0;
    else
    if(indentnum&0x40)
    {
        status(font_msg);
        myread_check(&fheader,sizeof fheader);
        for(i=0;i<sizeof fheader;i++) ozwritecustomfontbyte(0,i,((byte*)&fheader)[i]);
        if(fheader.length>MAX_CUSTOM_SLOT_SIZE*2) err(error_font);
        for(i=sizeof fheader;i<fheader.length;i++)
        {
            myread_check(&c,1);
            ozwritecustomfontbyte(0,i,c);
        }
        myfont=FONT_CUSTOM0;
    }
    if(myfont!=0xFF)
    {
        ozscancustomfonts();
        font_height=ozgetfontheight(myfont);
    }
    else
    {
        font_height=8;
        myfont=FONT_PC_NORMAL;
    }
    setnumlines();
    if(indentnum&0x20)
    {
        status(ext_msg);
        myread_check(&chartablesize,1);
        if(chartablesize!=CHARTABLE_MAX)
          errread();
        myread_check(chartable,chartablesize);
        ext_compare='|';
    }
    else ext_compare=0;
    dorev=indentnum&0x10;
    if(dorev)
    {
        extern byte arrow_1,arrow_2;
        arrow_1=0;
        arrow_2=0x36;
    }
    indentnum&=0xf;
    searchbuf_backup[0]=0;
    if(indentnum&0x8)
    {
        /* additional options */
        indentnum&=7;
        myread_check(&c,1);
        if(c&1) /* index names */
        {
            index_offset=tell1(); /* current position */
            index_area_length=header_length-index_offset;
        }

    }

}
