#include <string.h>
#include <oz.h>

#define MAXCOLUMNS 256
#define MAXLENGTH 2048

unsigned rows;
unsigned columns;

typedef char fname[MAX_FILENAMELEN+1];
char *selectname(void)
{
    static fname files[9];
    static byte refresh;
    static byte pos;
    static unsigned i;
    static byte x;
    static unsigned k;
    static unsigned curpos;
    register char *f;
    curpos=0;
    f=ozgetnextfilename(FIND_RESET | FIND_OWNED);
    if(f==NULL)
    {
        ozcls();
        ozputs(0,0,"No pix files found!");
        ozgetch();
        return NULL;
    }
    do
    {
        ozcls();
        for(pos=i=0;i<9;i++,pos+=8)
        {
            if(i && NULL==(f=ozgetnextfilename(FIND_OWNED))) break;
            strcpy(files[i],f);
            x=ozputch(0,pos,i+'1');
            x=ozputs(x,pos,". ");
            ozputs(x,pos,f);
        }
        curpos+=9;
        if(NULL!=f) f=ozgetnextfilename(FIND_OWNED);
        if(NULL!=f)
        {
           if(curpos>9)
             ozputs(0,72,"1-9: select, PageUp/PageDown: navigate, ESC: cancel");
           else
             ozputs(0,72,"1-9: select, PageDown: next page, ESC: cancel");
        }
        else
        {
            if(curpos>9)
              ozputs(0,72,"1-9: select, PageUp: previous page, ESC: cancel");
            else
              ozputs(0,72,"1-9: select, ESC: cancel");
        }
        refresh=0;
        do
        {
          k=ozngetch();
          if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC) return NULL;
          if(k==KEY_PAGEUP && curpos>=18)
          {
            curpos-=18;
            f=ozgetnextfilename(FIND_RESET | FIND_OWNED);
            for(i=0;i<curpos;i++)
              f=ozgetnextfilename(FIND_OWNED);
            refresh=1;
          }
          if(k==KEY_PAGEDOWN) refresh=1;
          if(f==NULL) refresh=0;
          if(k>='1' && k<'1'+i) refresh=1;
        } while(!refresh);
    } while(k<'1' || k>='1'+i);
    return files[k-'1'];
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

struct
{
    byte slug;
    unsigned length;
    unsigned offset;
} data[MAXCOLUMNS];

unsigned mygetauxsize(void);
void mywriteauxmem(unsigned offset,byte *data,unsigned length);
void myreadauxmem(unsigned offset,byte *data,unsigned length);

#ifndef CBASED_OUTPUT
extern byte one_column[MAXLENGTH];
void show_column(byte x,byte _slug,unsigned _start,unsigned _length);

#else
byte one_column[MAXLENGTH];

void show_column(byte x,byte _slug,unsigned _start,unsigned _length)
{
    static byte slug,value,count;
    static byte length;
    register byte *s=one_column;
    static unsigned screen_position;
    static unsigned start;
    screen_position=x;
    slug=_slug;
    length=_length;
    start=_start;
    while(start)
    {
        if(*s==slug)
        {
            value=*++s;
            count=*++s;
            start--;
            while(count && start)
            {
                start--;
                count--;
            }
            while(count && length)
            {
                length--;
                count--;
                ozdisplayputbyte(screen_position,value);
                screen_position+=30;
            }
        }
        else start--;
        s++;
    }
    while(length)
    {
        if(*s!=slug)
        {
            ozdisplayputbyte(screen_position,*s);
            length--;
        }
        else
        {
            ozdisplayputbyte(screen_position,(value=*++s));
            count=*++s;
            length--;
            while(count && length)
            {
                screen_position+=30;
                length--;
                count--;
                ozdisplayputbyte(screen_position,value);
            }
        }
        s++;
        screen_position+=30;
    }
}

#endif

void showat(int x, int y)
{
    static byte col,width;
    static unsigned length;
    static unsigned screen_pos;
    if(columns-x<30) width=columns-x; else width=30;
    if(rows-y<80) length=rows-y; else length=80;
/*    ozputs(20,20,utoa(length));
    ozgetch(); */
    for(col=0;col<width;col++)
    {
        myreadauxmem(data[col+x].offset,one_column,data[col+x].length);
        show_column(col,data[col+x].slug,y,length);
    }
    for(;col<30;col++)
        for(screen_pos=col;screen_pos<2400;screen_pos+=30)
            ozdisplayputbyte(screen_pos,0);
    for(screen_pos=29;screen_pos<2400;screen_pos+=30)
        ozdisplayandbyte(screen_pos,0x7F);  /* kill bleeding column */
}


int ozgets(byte _x,byte y,char *_s,int length)
{
	static byte x;
	static int stringpos;
	static unsigned k;
	register char *s=_s;
	x=_x;
	while(KEY_LOWER_ENTER!=(k=getch()) && k!=KEY_UPPER_ENTER
      && k!='\r' && k!='\n')
	{
		switch(k)
		{
            case 27:
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
				 return -1;
            case KEY_MAIN:
            case KEY_CALENDAR:
            case KEY_POWER:
            case KEY_TELEPHONE:
            case KEY_MEMO:
                ozexitto(k);
            case KEY_BACKLIGHT:
                oztogglelight();
            case KEY_BACKSPACE:
				 if(stringpos==0) break;
                 x-=ozputs(0,-1, s + (--stringpos) );
				 s[stringpos]=0;
                 ozputs(x,y,"  ");
                 break;
			default:
				 if(stringpos>=length-1) break;
				 x=ozputch(x,y,k);
				 s[stringpos++]=k;
				 s[stringpos]=0;
		}
	}
	return stringpos;
}

void help(void)
{
    ozcls();
    ozputs(0,0,"Pix: Picture Viewer Help");
    ozputs(0,8,"H / MENU: this help screen");
    ozputs(0,16,"arrow keys: scroll");
    ozputs(0,24,"2nd+arrow keys: move to edge of image");
    ozputs(0,32,"page-up/page-down: scroll up/down by a page");
    ozputs(0,40,"X: toggle display of coordinates");
    ozputs(0,48,"DELETE: delete this image from flash");
    ozputs(0,56,"L: load another image");
    ozputs(0,64,"Q / ESC: quit");
    ozputs(0,72,"Press any key to continue");
    ozngetch();
    return;
}

main()
{
    static int x,y;
    static byte c;
    static unsigned maxdatasize;
    register char *f;
    static unsigned curoffset;
    static int i;
    static byte hidexy=1;
    static char name[MAX_FILENAMELEN+1];
    ozsinglepage();
    maxdatasize=mygetauxsize();
START:
    ozcls();
    if(NULL==(f=selectname())) return;
    strcpy(name,f);
    ozcls();
    ozputs(ozputs(0,0,"Opening "),0,f);
    if(open1ro(name,O_RDONLY)<0 || read1ro(&c,1)!=1 || c!=0xA9)
    {
        ozputs(0,20,"Cannot open or invalid signature!");
        ozgetch();
        return;
    }
    read1ro(&rows,sizeof(rows));
    read1ro(&columns,sizeof(columns));
    if(columns>MAXCOLUMNS)
    {
        ozputs(0,20,"Image too wide!");
        ozgetch();
        return;
    }
    curoffset=0;
    for(i=0;i<columns;i++)
    {
        read1ro(&(data[i].length),sizeof(data[i].length));
        if(data[i].length+curoffset>maxdatasize || data[i].length>MAXLENGTH)
        {
            ozputs(0,20,"Out of memory!");
            ozgetch();
            return;
        }
        read1ro(&(data[i].slug),1);
        data[i].offset=curoffset;
        if(read1ro(one_column,data[i].length)<data[i].length)
        {
            ozputs(0,20,"Read error!");
            ozgetch();
            return;
        }
        mywriteauxmem(curoffset,one_column,data[i].length);
        curoffset+=data[i].length;
    }
    close1ro();
    x=y=0;
    ozcls();
    do
    {
        if(x<0) x=0;
        if(x>=columns-29)
        {
            x=columns-29;
            if(x<0) x=0;
        }
        if(columns<=29) x=0;
        if(y<0) y=0;
        if(y>=rows-80)
        {
            y=rows-80;
            if(y<0) y=0;
        }
        if(rows<=80) y=0;
        showat(x,y);
        if(!hidexy)
        {
            static char buf[14]="(";
/*            buf[0]='('; */
            strcpy(buf+1,utoa(x*8));
            strcat(buf,",");
            strcat(buf,utoa(y));
            strcat(buf,")");
            ozputs(0,0,buf);
        }
        switch(ozngetch())
        {
            case KEY_LEFT: x--; break;
            case KEY_UP: y-=8; break;
            case KEY_RIGHT: x++; break;
            case KEY_DOWN: y+=8; break;
            case 'x': hidexy=!hidexy; break;
            case '\8':
                ozputs(0,0,"Are you sure you want to delete this picture?");
                if(ozngetch()=='y')
                {
                    unlink(name);
                    return;
                }
                break;
            case KEY_UPPER_ESC:
            case KEY_LOWER_ESC:
            case 'q': return;
            case 'l': goto START;
            case KEY_LOWER_MENU:
            case KEY_UPPER_MENU:
            case 'h': help(); break;
            case KEY_2ND:
                 switch(ozngetch() & 0xF0FF)
                 {
                    case KEY_PAGEUP:
                    case KEY_UP: y=0; break;
                    case KEY_PAGEDOWN:
                    case KEY_DOWN: y=rows-1; break;
                    case KEY_LEFT: x=0; break;
                    case KEY_RIGHT: x=columns-1; break;
                 }
                 break;
            case KEY_LEFT_SHIFT:
            case KEY_RIGHT_SHIFT:
                switch(ozngetch() & 0xF0FF)
                {
                    case KEY_PAGEUP: y=0; break;
                    case KEY_PAGEDOWN: y=rows-1; break;
                    case KEY_LEFT: x-=29; break;
                    case KEY_RIGHT: x+=29; break;
                    case KEY_DOWN: y+=72; break;
                    case KEY_UP: y-=72; break;
                }
                break;

            case MASKSHIFT | KEY_PAGEUP: y=0; break;
            case MASKSHIFT | KEY_PAGEDOWN: y=rows-1; break;
            case MASKSHIFT | KEY_LEFT: x-=29; break;
            case MASKSHIFT | KEY_RIGHT: x+=29; break;
            case MASKSHIFT | KEY_DOWN: y+=72; break;
            case MASKSHIFT | KEY_UP: y-=72; break;

            case MASK2ND | KEY_LEFT: x=0; break;
            case MASK2ND | KEY_RIGHT: x=columns-1; break;
            case MASK2ND | KEY_PAGEUP:
            case MASK2ND | KEY_UP: y=0; break;
            case MASK2ND | KEY_PAGEDOWN:
            case MASK2ND | KEY_DOWN: y=0; break;
            case KEY_PAGEUP: y-=72; break;
            case KEY_PAGEDOWN: y+=72; break;
        }
    } while(1);
}

