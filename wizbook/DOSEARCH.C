#include <oz.h>
#include <string.h>
#include "wizbook.h"
#define warnanykey(s) ozwarn(s,anykey)


unsigned long saved_last;
byte searching;


extern char *caseit;
extern byte dorev;

byte dictsearch;

static byte notinterrupted;
/* char searchbuf[SEARCHBUF+1]; */
/* char searchbuf_backup[SEARCHBUF+1]; */
char *swords[MAX_SWORDS];
byte wilds[MAX_SWORDS];
unsigned long found[MAX_SWORDS];
unsigned numswords;
byte nfound;
#define NOTFOUND ((unsigned long)20000000l)
static byte cursor=239;


void docursor(byte c)
{
    _ozfilledbox(cursor,8,1,font_height,c);
}

void showstring(void)
{
    byte x;
    setmyfont();
#if 0
    if(dorev)
    {
        strcpy(line_buf,searchbuf);
        ozputsf(8);
    }
    else cursor=ozputs(0,8,searchbuf);
#endif
    strcpy(line_buf,searchbuf);
    x=ozputsf(8);
    if(!dorev) cursor=x;
    setnormfont();
}

void getsearchbuf(void)
{
    register char *p=searchbuf;
    static byte len;
    static unsigned k;
    static byte clr;
    clear240(0,2*line_height);
    ozputs00("Search for:");
    strcpy(p,searchbuf_backup);
    clr=1;
    len=strlen(p);
    p+=len;
    if(len)
    {
        showstring();
        _ozfilledbox(0,8,cursor+1,font_height,XOR);
        k=mygetch();
        if(k!=KEY_RIGHT && k!='\r' && k!=KEY_LEFT)
        {
            len=0;
            p=searchbuf;
            *searchbuf=0;
            ozungetch(k);
        }
        else if(k=='\r') ozungetch('\r');
    }
    for(;;)
    {
        if(clr)
        {
            clear240(8,font_height);
            clr=0;
        }
        showstring();
        docursor(1);
        switch(k=mygetch())
        {
            case KEY_LEFT:
            case KEY_DELETE:
                if(p>searchbuf)
                {
                    *--p=0;
                    len--;
                    clr=1;
                }
                break;
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
                if(!len) return;
                *searchbuf=0;
                p=searchbuf;
                len=0;
                clr=1;
                break;
            case '\r':
                strcpy(searchbuf_backup,searchbuf);
                caseit=searchbuf;
                raisecase();
                docursor(0);
                return;
            default:
                if(len<SEARCHBUF && isalnum0(k))
                {
                   *p++=k;
                   len++;
                   *p=0;
                }
        }
    }
}

void parsesearchbuf(void)
{
    register char *p=searchbuf;
    if(*p=='-')
    {
        dictsearch=1;
        p++;
    }
    else dictsearch=0;
    for(numswords=0;numswords<MAX_SWORDS;)
    {
        while(*p=='.' || *p==' ') p++;
        if(!*p) break;
        swords[numswords]=p;
        while(isalnum(*p)) p++;
        wilds[numswords++] = *p=='.';
        if(!*p) break;
        *p++=0;
    }
}

unsigned ybar(byte spread)
{
    return (screen_offset+buffer_offset-header_length)*spread/(length-header_length);
}

byte scan(void)
{
    static unsigned i;
    static byte line1;
    static byte y0;
    static byte paras;
    static unsigned k;
    static byte lstart;
    static byte oldindent;
    y0=ybar(80);
    paras=0;
    caseit=line_buf;
    notinterrupted=1;
    for(;;)
    {
        for(i=0;i<MAX_SWORDS;i++) found[i]=NOTFOUND;
        nfound=0;
        line1=1;
        if(!(paras++&(byte)(0xF)))
        {
            _ozvline(239,y0,ybar(80)-y0+1,1);
            if(ozkeyhit())
            {
                k=ozgetch();
                if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC)
                    return notinterrupted=0;
            }
        }
        while(0==(lstart=filebuf[screen_offset]) || line1)
        {
            if(dictsearch && !lstart) goto NEXT;
            line1=0;
            oldindent=indentnum;
            indentnum=0;
            dcompline(filebuf+screen_offset);
            indentnum=oldindent;
            raisecase();
            for(i=0;i<numswords;i++)
                if(found[i]==NOTFOUND && search(swords[i],wilds[i]))
                {
                   found[i]=buffer_offset+screen_offset;
                   nfound++;
                   if(nfound==numswords) return 1;
                }
            NEXT:
            checkshiftdown();
            if(!forwardline()) return 0;
        }
    }
}

void notfound(void)
{
   if(notinterrupted) warnanykey("Not found!");
}

unsigned long getsearchpos(void)
{
    static unsigned i;
    static unsigned long pos;
    pos=NOTFOUND;
    for(i=0;i<numswords;i++)
       if(found[i]<pos) pos=found[i];
    return pos;
}

static unsigned long spos;


void runsearch(void)
{
    if(!scan())
    {
        notfound();
        spos=saved_last;
    }
    else
        spos=getsearchpos();
    goto_pos(spos);
    searching=0;
}

static void initsearch(void)
{
    saved_last=thepos;
    searching=1;
}

void dosearch_from(unsigned long start)
{
    getsearchbuf();
    parsesearchbuf();
    if(numswords==0) return;
    initsearch();
    goto_pos(start);
    runsearch();
}

void searchagain(void)
{
    if(nfound<numswords)
    {
        notfound();
        return;
    }
    if(!numswords) return;
    initsearch();
    goto_pos(getsearchpos());
    checkshiftdown();
    if(!forwardline()) return;
    runsearch();
}

void searchtobookmarksfrom(unsigned long start)
{
    static unsigned i;
    byte f;
    byte found;
 /*   putformatted(0,0,"^ ",start);
    ozgetch(); */
    found=0;
    i=numbookmarks;
    getsearchbuf();
    parsesearchbuf();
    if(numswords==0) return;
    initsearch();
    goto_pos(start);
    while(scan())
    {
        if(numbookmarks>=MAX_BOOKMARKS)
        {
            ozwarn("Too many bookmarks!",anykey);
            break;
        }
        if(!found)
        {
            bookmarkptr=numbookmarks;
            found=1;
        }
        goto_pos(bookmarks[numbookmarks++]=getsearchpos());
        while((f=forwardline()) && filebuf[screen_offset]==0);
        /* skip to new paragraph */
        if(!f) break;
    }
    if(!found) notfound();
    goto_pos(saved_last);
    searching=0;
}
