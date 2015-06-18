#include <oz.h>
#include "edit.h"

/* #define OZSNAP /* */

byte islocked;
extern byte searchmode;
unsigned long start_time;
struct head_struct head0;

byte ozgetindexby(byte itype);
void ozsetindexby(byte itype,byte c);
byte ozgetappfont(void);
void ozsetappfont(byte c);
int strlen(char *s);
int readmemoindex(byte inum);

char sstring[]="Search string: ";
char fatal_message[]="Fatal error!";

static byte after_search;
extern char search_string[];
unsigned search_next=0;
unsigned found;
char special_header[SPECIAL_LENGTH+1]="zzzz\xFD!";
char special_date[]="20991231";
byte indexnum=0;
unsigned goto_recnum;
static unsigned offset;
static unsigned found_loc;
static byte pos;
extern char title[TITLE_LENGTH+1];
extern char date[9];
static old_offset=0xFFFF;
static old_pos=0xFF;
byte typein;
extern char typedin[TITLE_LENGTH+1];

void time_and_edit(void)
{
    head0.start_time=oztime_fake();
    edit();
    if(modified) search_next=0;
    modified=0;
}

char low(char c)
{
    if(c>='A' && c<='Z') return c+'a'-'A';
    else return c;
}

void select(byte pos)
{
   _ozfilledbox(0,10+pos*10,239,10,XOR);
}

unsigned searchit(unsigned _start)
{
    static unsigned i;
    static byte l;
    static unsigned k;
    quick_index=0;
    l=strlen(search_string);
    restore_clipboard();
    for(i=_start;i<start_extended;i++)
    {
        if(ozkeyhit())
        {
            k=ozgetch();
            if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC) break;
        }
        searchmode=1;
        if(load_or_delete_memo(i,0)<0 || file_length<l)
            continue;
        if( 0xFFFF!=(found_loc=search(0,l)) )
        {
            save_clipboard();
            readmemoindex(indexnum);
            searchmode=0;
            return recnums[0];
        }
    }
    beep();
    save_clipboard();
    readmemoindex(indexnum);
    searchmode=0;
    return 0;
}

void find_position0(unsigned n)
{
    if(n==start_extended)
    {
        pos=offset=0;
    }
    else if(n<pos)
    {
        offset=0;
        pos=n;
    }
    else offset=n-pos;
    if(start_extended<=7)
    {
        pos=pos+offset;
        offset=0;
    }
    else if(start_extended-offset-pos<7)
    {
        offset=start_extended-7;
        pos=n-offset;
    }
}

void find_position(unsigned rn)
{
    static unsigned n;
    for(n=0;n<start_extended;n++)
        if(rn==ozreadauxword2(n)) break;
    find_position0(n);
}

void clt(void)
{
    if(typein)
    {
        typein=0;
        *typedin=0;
        cleartop();
        head();
    }
}

void totop(void)
{
   clt();
   offset=pos=0;
}

void tobot(void)
{
    clt();
    if(start_extended<7)
    {
        offset=0;
        if(start_extended) pos=start_extended-1;
          else pos=0;
    }
    else
    {
        offset=start_extended-7;
        pos=6;
    }
}

void tosearch(void)
{
    static byte x;
    if(!start_extended) return;
    clt();
    cleartop();
    x=ozputs0(0,sstring);
    old_offset=0xFFFF;
    if(ozeditline1(x,0,search_string,SEARCH_STRING_MAX,238-x)
     <= 0)
        return;
    restore_clipboard();
    if(found=searchit(0))
    {
        find_position(found);
        search_next=offset+pos+1;
        after_search=2;
    }
    else search_next=0;
    save_clipboard();
}

void tosearchagain(void)
{
    if(!start_extended) return;
    clt();
    if(!*search_string) return;
    restore_clipboard();
    if(found=searchit(search_next))
    {
        find_position(found);
        search_next=offset+pos+1;
        after_search=2;
    }
    else search_next=0;
    save_clipboard();
}

byte matchi(char *_s,char *t)
{
    static byte c;
    register char *s=_s;
    c=0;
    while(*s)
    {
        if(low(*s++)==low(*t++)) c++;
          else break;
    }
    return c;
}

void find_typedin(unsigned start)
{
    static unsigned n;
    static int d;
    static byte i;
    static char t0;
    static byte match_count;
    t0=typedin[PRELOAD_LEN];
    typedin[PRELOAD_LEN]=0;
    n=0;
    for(i=0;i<PRELOAD_NUM;i++)
    {
        if(stricmp(preload_titles[i],typedin)>=0)
        {
            n=PRELOAD_POS(i-1);
            break;
        }
    }
    if(i==PRELOAD_NUM) n=PRELOAD_POS(PRELOAD_NUM-1);
    typedin[PRELOAD_LEN]=t0;
    if(n<start) n=start;
    for(;n<start_extended;n++)
    {
        readmemotitledate(n);
        title[typein]=0;
        d=stricmp(title,typedin);
        if(d==0) break;
        if(d>0)
        {
            match_count=matchi(title,typedin);
            if(n>start)
            {
                readmemotitledate(n-1);
                if(match_count<=matchi(title,typedin)) n--;
            }
            break;
        }
    }
    if(n==start_extended) n--;
    find_position0(n);
}

int del(void)
{
    clt();
    if(warn("Really delete?",
         yn_message)!=KEY_LOWER_ENTER)
    {
        old_offset=0xFFFF;
        return 0;
    }
    if(pos+offset+1<start_extended)
        goto_recnum=ozreadauxword2((offset+pos+1));
    else if(pos+offset>0)
        goto_recnum=ozreadauxword2((offset+pos-1));
    else
        goto_recnum=0;
    load_or_delete_memo(offset+pos,1); /* should be 1 to really del */
    return 1;
}

void totypeinbs()
{
    if(!typein) return;
    if(typein==1)
    {
        clt();
        return;
    }
    typedin[--typein]=0;
    cleartop();
    ozputs0(0,typedin);
    find_typedin(0);
}

int browse(void)
{
    static unsigned key;
    static new_loc;
    typein=0;
    old_offset=0xFFFF;
    after_search=0;
    for(;;)
    {
        oz64hztimeroff();
        if(old_offset!=offset)
        {
            if(after_search>0) after_search--;
            if(old_offset-1==offset)
            {
                select(old_pos);
                scr_down();
                cleararea(11*30,10*30);
                putmemotitle(11,offset);
            }
            else if(old_offset!=0xFFFF && old_offset+1==offset)
            {
                select(old_pos);
                scr_up();
                cleararea(2400-9*30,9*30);
                putmemotitle(11+10*6,offset+6);
            }
            else
                showmemos(offset);
            old_offset=offset;
            stripe();
            select(pos);
            ozsetmemolastrecnum(ozreadauxword2((offset+pos)));
            old_pos=pos;
        }
        else if(old_pos!=pos)
        {
            if(after_search>0) after_search--;
            select(old_pos);
            select(pos);
            ozsetmemolastrecnum(ozreadauxword2((offset+pos)));
            old_pos=pos;
        }
        cleararrows();
        if(offset>0) uparrow();
        if(offset+7<start_extended) downarrow();
        key=ozgetch();
        switch(key)
        {
            case KEY_LOWER_MENU:
            case KEY_UPPER_MENU:
                donation_info(0);
                old_offset=0xFFFF;
                break;
            case KEY_2ND:
                switch(ozgetch() & 0xF0FF)
                {
                    case KEY_UP:
                    case KEY_PAGEUP:
                        totop();
                        break;
                    case KEY_DOWN:
                    case KEY_PAGEDOWN:
                        tobot();
                        break;
                    case 's':
                        installcursor();
                        tosearch();
                        break;
                    case 'g':
                        tosearchagain();
                        break;
#ifdef OZSNAP
                    case 'i':
                        ozsnap();
                        ozsnap();
                        break;
#endif
                }
                break;
            case MASK2ND | KEY_UP:
            case MASK2ND | KEY_PAGEUP:
                totop();
                break;
            case MASK2ND | KEY_DOWN:
            case MASK2ND | KEY_PAGEDOWN:
                tobot();
                break;
            case MASK2ND | 's':
                installcursor();
                tosearch();
                break;
            case MASK2ND | 'g':
                tosearchagain();
                break;
            case KEY_CATEGORY:
                clt();
                goto_recnum=ozreadauxword2((pos+offset));
                indexnum=!indexnum;
                ozsetindexby(TYPENUM_MEMO,indexnum+1);
                pos=offset=0;
                return 1;
            case KEY_TELEPHONE:
            case KEY_MAIN:
            case KEY_SCHEDULE:
            case KEY_MEMO:
            case KEY_MYPROGRAMS:
                make_clip_permanent();
                ozexitto(key);
            case KEY_UPPER_ESC:
            case KEY_LOWER_ESC:
                make_clip_permanent();
                return 0;
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
            case 13:
                if(!start_extended) break;
                clt();
                restore_clipboard();
                if(load_or_delete_memo(offset+pos,0)<0)
                {
                    if(!(issecret && islocked))
                    {
                        beep();
                        warn(fatal_message,anykey_message);
                    }
                    old_pos=0xFFFF;
                    return 1;
                }
                goto_recnum=recnums[0];
                if(after_search)
                {
                    last_found=found_loc;
                }
                else last_found=0xFFFF;
                time_and_edit();
                return 1;
            case KEY_NEW:
                clt();
                restore_clipboard();
                goto_recnum=0;
                num_recnums=0;
                file_length=0;
                last_found=0xFFFF;
                issecret=0;
                time_and_edit();
                return 1;
            case KEY_BACKSPACE:
                if(typein) totypeinbs();
                else
                {
                    clt();
                    if(del()) return 1;
                    old_offset=0xFFFF;
                    break;
                }
                break;
            case KEY_PAGEUP:
                clt();
                if(offset+pos<7) pos=0;
                if(offset>7) offset-=7; else offset=0;
                break;
            case KEY_PAGEDOWN:
                clt();
                new_loc=offset+pos+7;
                if(start_extended<7)
                {
                    offset=0;
                    if(start_extended>0) pos=start_extended-1;
                      else pos=0;
                    break;
                }
                offset+=7;
                if(offset>=start_extended-7) offset=start_extended-7;
                if(new_loc>=start_extended)
                {
                    if(start_extended>0) pos=start_extended-offset-1;
                      else pos=0;
                }
                break;
            case KEY_UP:
                clt();
                if(offset==0 && pos==0) break;
                if(pos>0)
                    pos--;
                else
                    offset--;
                break;
            case KEY_DOWN:
                clt();
                if(pos+offset+1>=start_extended) break;
                if(pos==6)
                {
                    offset++;
                    if(offset>=start_extended)
                    {
                       if(start_extended>7) offset=start_extended-7;
                          else offset=0;
                       if(start_extended>offset) pos=start_extended-offset-1;
                          else pos=0;
                    }
                }
                else pos++;
                break;
            case KEY_LEFT:
                totypeinbs();
                break;
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            case '-':
                clicktoggle();
                break;
            default:
                if(!start_extended) break;
                if(!indexnum || key<32 || key>127 || typein==TITLE_LENGTH) break;
                if(!typein) offset=pos=0;
                typedin[typein++]=key;
                typedin[typein]=0;
                cleartop();
                ozputs0(0,typedin);
                find_typedin(offset+pos);
                break;
        }
    }
}

unsigned ozcrc16auxmem(unsigned _pos, unsigned _length);

#define SAVEKEY 0x6DBE /* must be bigger than max clipboard size */

void exitsave(void)
{
    if(file_length>16378) file_length=16378; /* lose some stuff */
    if(modified)
    {
        ozwriteauxword(16378,file_length);
        ozwriteauxword(16380,ozcrc16auxmem(0,file_length));
        ozwriteauxword(16382,SAVEKEY);
        insert(0,16378-file_length); /* shift over to end of aux memory */
    }
    else
    {
        if(ozreadauxword(16382)==SAVEKEY) ozwriteauxword(16382,0);
    }
}


main()
{
    init();
    do
    {
       if(readmemoindex(indexnum)<0)
       {
          beep();
          warn(fatal_message,anykey_message);
          return;
       }
       if(goto_recnum) find_position(goto_recnum);
         else offset=pos=0;
    } while(browse());
}

