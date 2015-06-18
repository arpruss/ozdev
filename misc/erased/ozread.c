#include <stdlib.h>
#include <oz.h>
#include <ozread.h>

/* #define DEBUG */

#define MAXOZFILES 1
#define CLUSTERDATALEN 62

struct ozfile
{
    unsigned startloc;
    unsigned curloc;
    unsigned dir_page;
    unsigned dir_offset;
    byte cbuf[62];
    byte lastcluster;
    byte curpos;
};

static byte init=0;
static struct ozfile ozfiles[MAXOZFILES];

#ifdef DEBUG
char nibble(unsigned char c)
{
    if(c<10) return '0'+c;
     else return 'a'+c-10;
}

char *hex(unsigned char c)
{
    static char buf[3];
    buf[2]=0;
    buf[1]=nibble(c&0xF);
    buf[0]=nibble(c>>4);
    return buf;
}
#endif

int
ozopenread(char *name)
{
    int i;
    char *b;
    unsigned loc;
    unsigned recnum;
    if(!init)
    {
        for(i=0;i<MAXOZFILES;i++)
          ozfiles[i].startloc=0;
        init=1;
    }
    for(i=0;i<MAXOZFILES;i++)
        if(ozfiles[i].startloc==0) break;
    if(i==MAXOZFILES)
    {
/*        ozputs(70,0,"Too many handles."); */
        return -1;
    }
    if(!(loc=ozfindmemo(name,&(ozfiles[i].dir_page),&(ozfiles[i].dir_offset),
            &recnum)))
    {
/*        ozputs(70,0,"Cannot find file."); */
        return -1;
    }
#ifdef DEBUG
    termputs("Found on page ");
    termputs(hex(loc>>8));
    termputs("\r\n");
#endif
    ozfiles[i].startloc=ozfiles[i].curloc=loc;
    ozfiles[i].curpos=0;
    b=ozloadcluster(loc);
/*    {extern char mybuf[]; int i; for(i=0;i<64;i++)
ttyprint(mybuf[i]);} */
    if(b[1]!='\xFF' || b[0]!='\xFF') ozfiles[i].lastcluster=0;
      else ozfiles[i].lastcluster=1;
    memcpy(ozfiles[i].cbuf,b+2,CLUSTERDATALEN);
    return i;
}

int
ozopenreadloc(unsigned loc)
{
    char *b;
    int i;
    if(!init)
    {
        for(i=0;i<MAXOZFILES;i++)
          ozfiles[i].startloc=0;
        init=1;
    }
    for(i=0;i<MAXOZFILES;i++)
        if(ozfiles[i].startloc==0) break;
    if(i==MAXOZFILES)
    {
/*        ozputs(70,0,"Too many handles."); */
        return -1;
    }
#ifdef DEBUG
    termputs("Found on page ");
    termputs(hex(loc>>8));
    termputs("\r\n");
#endif
    ozfiles[i].startloc=ozfiles[i].curloc=loc;
    ozfiles[i].dir_page=ozfiles[i].dir_offset=0;
    ozfiles[i].curpos=0;
    b=ozloadcluster(loc);
/*    {extern char mybuf[]; int i; for(i=0;i<64;i++)
ttyprint(mybuf[i]);} */
    if(b[1]!='\xFF' || b[0]!='\xFF') ozfiles[i].lastcluster=0;
      else ozfiles[i].lastcluster=1;
    memcpy(ozfiles[i].cbuf,b+2,CLUSTERDATALEN);
    return i;
}

int
ozclose(int handle)
{
    ozfiles[handle].startloc=0;
    return 0;
}

int ozgetc(int handle)
{
    char *b;
    int i;
    register struct ozfile *p=&(ozfiles[handle]);
    if(p->startloc==0 || (p->lastcluster &&
    p->curpos==CLUSTERDATALEN)) return -1;
    if(p->curpos==CLUSTERDATALEN)
    {
        b=ozloadcluster(p->curloc=ozinclocation(p->curloc));
        p->lastcluster=(b[1]!='\xFF' || b[0]!='\xFF')?0:1;
        memcpy(p->cbuf,b+2,CLUSTERDATALEN);
        p->curpos=0;
    }
    return (p->cbuf)[(p->curpos)++];
}

