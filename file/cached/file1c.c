#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"
#include "cache.h"

#define O_CHECKONLY 4

static char _buffer[MAX_WRITEBUFFERLEN];

ozfile_t _oz_the_file;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
static byte init=0;

unsigned _ozfile1cache[CACHE_LEN];
char cached_name[MAX_FILENAMELEN]="";

void _ozreadfirstcluster1c(unsigned loc, char *firstcluster);
unsigned __ozfileheaderdecode(cfile_header_t *_p, unsigned *length_ptr);
byte ozflashread(unsigned p, unsigned a);
int unlink(char *name);
int _ozkill(unsigned file_id);

/* random number routines lifted from Hi-Tech C library */

static long    __seedrandx = 1;
static byte random_init=0;

int close1c(void);

static void initfiles(void)
{
    init=1;
    _oz_the_file.buffer=NULL;
    atexit(close1c);
}

static utoa2digit(char *s, unsigned u)
{
    s[0]=(u/10)+'0';
    s[1]=(u%10)+'0';
}     

static void encode(cfile_header_t *_p, unsigned owner_id, unsigned length)
{
    register cfile_header_t *p=_p;
    /* 01234567 (fake_time) */
    /* HHMMHHMM */
    /* 0123 (fake_alarm) */
    /* HHMM */
   (p->fake_time)[8]=(p->fake_alarm)[4]=0;
   utoa2digit((p->fake_alarm)+2,length%60);
   length/=60;
   utoa2digit(p->fake_alarm,length%24);
   length/=24;
   (p->fake_time)[1]=length+'0';  /* maximum length: 5759 */
   utoa2digit((p->fake_time)+6,owner_id%60);
   owner_id/=60;
   utoa2digit((p->fake_time)+4,14+owner_id%10);
   owner_id/=10;
   utoa2digit((p->fake_time)+2,owner_id%60);
   owner_id/=60;
   /* now owner_id is either 0 or 1 */
   (p->fake_time)[0]=owner_id+'0';
}

static mysrand(void)
{
    random_init=1;
    __seedrandx = ozsec()+60*ozmin()+3600l*ozhour()+86400l*ozday();
}

static myrand()
{
    return(((__seedrandx = __seedrandx*1103515245l + 12345l)>>16) &
    077777l);
}

int close1c()
{
    register cfile_header_t *header;
    static int rv;
    if(_oz_the_file.buffer==NULL) return -1;
    if(_oz_the_file.mode & O_RDONLY)
    {
        _oz_the_file.buffer=NULL;
        return 0;
    }
    else if(_oz_the_file.mode & O_WRONLY)
    {
        rv=0;
        header=(cfile_header_t *)(_oz_the_file.buffer);
        encode(header,_ozfileowner,_oz_the_file.cur_partoffset);
        header->last_part=1;
        if(ozwritefile(_oz_the_file.buffer,_oz_the_file.cur_partoffset,TYPENUM_SCHEDULE))
           rv=-1;
        _oz_the_file.buffer=NULL;
        return rv;
    }
}

static void __ozreadnotfirstpart1c(unsigned loc, char
*firstcluster)
{
    register cfile_header_t *header;
    header=(cfile_header_t *)(2+firstcluster);
    memcpy(_oz_the_file.buffer,header,62);
    if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
      _oz_the_file.last_cluster_of_part=1;
    else
      _oz_the_file.last_cluster_of_part=0;
    _oz_the_file.cur_part=header->part_number;
    _oz_the_file.last_part=header->last_part;
    _oz_the_file.cur_clusteroffset=_oz_the_file.cur_partoffset=sizeof(*header);
    __ozfileheaderdecode(header,&(_oz_the_file.part_length));
    _oz_the_file.loc=loc;
}

unsigned __ozloadpart1c(unsigned part_number)
{
    static unsigned loc;
    static unsigned page,offset,recnum;
    static cfile_header_t *header;
    register char *firstcluster;
    if(part_number<CACHE_LEN && (loc=_ozfile1cache[part_number]))
    {
        header=(void*)(2+(firstcluster=ozloadcluster(loc)));
        if(header->part_number == part_number)
        {
           __ozreadnotfirstpart1c(loc,firstcluster);
           return loc;
        }
    }
    page=offset=0;
    while((loc=ozfindfileid(_oz_the_file.file_id,&page,&offset,&recnum)))
    {
        header=(void*)(2+(firstcluster=ozloadcluster(loc)));
        if(header->part_number != part_number)
           continue;
        __ozreadnotfirstpart1c(loc,firstcluster);
        if(part_number<CACHE_LEN) _ozfile1cache[part_number]=loc;
        break;
    }
    return loc;
}



void _ozreadfirstcluster1c(unsigned loc, char *firstcluster)
{
    register cfile_header_t *header;
    header=(cfile_header_t *)(2+firstcluster);
    memcpy(_oz_the_file.buffer,firstcluster+2,62);
    if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
        _oz_the_file.last_cluster_of_part=1;
    else _oz_the_file.last_cluster_of_part=0;
    _oz_the_file.last_part=header->last_part;
    _oz_the_file.owner_id=__ozfileheaderdecode(header,&(_oz_the_file.part_length));
    _oz_the_file.first_part_length=_oz_the_file.part_length;
    _oz_the_file.file_id=header->file_id;
    _oz_the_file.cur_clusteroffset=_oz_the_file.cur_partoffset=sizeof(*header);
    _oz_the_file.loc=_oz_the_file.startloc=loc;
    _ozfile1cache[0]=loc;
    _oz_the_file.mode=O_RDONLY;
    _oz_the_file.cur_part=0;
    _oz_the_file.curpos=0;
}

static int extant(unsigned id)
{
        if(_oz_the_file.buffer != NULL
         && _oz_the_file.mode == O_WRONLY
         && _oz_the_file.file_id == id) return 1;
    return 0;
}

static int generate_id(unsigned *id)
{
    static unsigned end,page,offset,recnum;
    page=offset=0;
    if(!ozfindfileid(*id,&page,&offset,&recnum) && !extant(*id))
      return 1;
    if(!random_init) mysrand();
    *id+=myrand();
    end=(*id)-1;
    for(;(*id)!=end;(*id)++)
    {
        page=offset=0;
        if(!ozfindfileid(*id,&page,&offset,&recnum) && !extant(*id))
          return 1;
    }
    return 0; /* This should never happen, but let's keep compilers happy */
}

int open1c(char *_name, int mode)
{
    register cfile_header_t *header;
    static char *firstcluster;
    static char name[MAX_FILENAMELEN+1];
	static unsigned loc,page,offset,recnum;
    static unsigned id,found_id;
    static unsigned i;
	page=offset=0;
    if(!init)
        initfiles();
    strncpy(name,_name,MAX_FILENAMELEN);
    name[MAX_FILENAMELEN]=0;
    if(_oz_the_file.buffer!=NULL) return -1;
    if(!(mode & O_CHECKONLY))
        if(_oz_the_file.buffer!=NULL
          && !strcmp(_oz_the_file.filename,name)) return -1;
    if((mode & O_RDONLY) || (mode & O_CHECKONLY))
	{
        while(loc=ozfindfilename(name,&page,&offset,&recnum))
		{
            if(mode & O_CHECKONLY) return 0;
/*            firstcluster=ozloadcluster(loc);
             header=(cfile_header_t *)(2+firstcluster); */
            if(NULL==(_oz_the_file.buffer=_buffer))
            {
                return -1;
            }
            _oz_the_file.mode=O_RDONLY;
            if(strcmp(cached_name,name))
            {
                for(i=0;i<CACHE_LEN;i++) _ozfile1cache[i]=0;
                strcpy(cached_name,name);
            }
            _ozreadfirstcluster1c(loc,ozloadcluster(loc));
            return 0;
		}
		return -1;
	}
	else if(mode & O_WRONLY)  /* really, this should check O_CREAT */
	{
        found_id=0;
        while(loc=ozfindfilename(name,&page,&offset,&recnum))
        {
            firstcluster=ozloadcluster(loc);
            header=(cfile_header_t *)(2+firstcluster);
            found_id=1;
            id=header->file_id;
            _ozkill(id);
            break;  /* remove for safety? */
        }
        if(!found_id)
        {
            id=_ozhashpjw(name);
            page=offset=0;
            if(!generate_id(&id)) return -1;
        }
        _oz_the_file.file_id=id;
        if(NULL==(_oz_the_file.buffer=_buffer))
            {
                return -1;
            }
        _oz_the_file.cur_part=0;
        _oz_the_file.curpos=0;
        _oz_the_file.mode=O_WRONLY;
        for(i=0;i<62;i++) (_oz_the_file.buffer)[i]=0;
        header=(cfile_header_t *)(_oz_the_file.buffer);
        strcpy(header->fake_time,"1200");
/*        header->part_number=0; */
        header->header_id='c';
        header->file_id=_oz_the_file.file_id;
        (header)->internal_file_id=0xFFFF;
        (header)->header_other[0]=0xD0;
        (header)->header_other[1]=0x0D;
/*        (header)->header_other[2]=0x00; */
        (header)->header_other[3]=0x16;
/*        (header)->header_other[4]=0x00; */
        (header)->header_other[5]=0x1F;
/*        (header)->header_other[6]=0x00; */
        strcpy(header->fake_date,"19010101");
        strcpy(header->filename,name);
        _oz_the_file.cur_partoffset=sizeof(*header);
        if(!strcmp(name,cached_name)) *cached_name=0; /* clear cache */
        return 0;
	}
}

int __read1c(char *data, int length, int skip)
{
    static int didread;
    static int curread;
    register char *firstcluster;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_RDONLY) return -1;
    didread=0;
    while(length>0)
    {
        if((_oz_the_file.cur_partoffset) >= (_oz_the_file.part_length))
		{
            if((_oz_the_file.last_part && _oz_the_file.last_cluster_of_part)
            || !__ozloadpart1c(_oz_the_file.cur_part+1))
            {
                return didread;
            }
        }
        if((_oz_the_file.cur_clusteroffset)>=MAX_READBUFFERLEN)
		{
            if(_oz_the_file.last_cluster_of_part)
            {
                if(_oz_the_file.last_part) return didread;
                  else continue; /* This shouldn't happen */
            }
            _oz_the_file.loc=ozinclocation(_oz_the_file.loc);
            firstcluster=ozloadcluster(_oz_the_file.loc);
			if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
                _oz_the_file.last_cluster_of_part=1;
            else _oz_the_file.last_cluster_of_part=0;
            memcpy(_oz_the_file.buffer,firstcluster+2,62);
            _oz_the_file.cur_clusteroffset=0;
		}
        curread=length;
        if(curread>MAX_READBUFFERLEN-(_oz_the_file.cur_clusteroffset))
          curread=MAX_READBUFFERLEN-(_oz_the_file.cur_clusteroffset);
        if(curread>(_oz_the_file.part_length)-(_oz_the_file.cur_partoffset))
          curread=(_oz_the_file.part_length)-(_oz_the_file.cur_partoffset);
        if(!skip) memcpy(data,_oz_the_file.cur_clusteroffset+_oz_the_file.buffer,curread);
        _oz_the_file.cur_clusteroffset+=curread;
        _oz_the_file.cur_partoffset+=curread;
		length-=curread;
		data+=curread;
		didread+=curread;
        _oz_the_file.curpos+=curread;
    }
	return didread;
}

int read1c(void *data, int length)
{
    return __read1c((char*)data,length,0);
}

int write1c(void *data, int length)
{
    static int didwrite;
    static int curwrite;
    register cfile_header_t *header;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_WRONLY) return -1;
    didwrite=0;
	while(length>0)
    {
        if(_oz_the_file.cur_partoffset >= MAX_WRITEBUFFERLEN)
        {
            header=(cfile_header_t *)(_oz_the_file.buffer);
            encode(header,_ozfileowner,MAX_WRITEBUFFERLEN);
            header->last_part=0; /* not last */
            if(ozwritefile(_oz_the_file.buffer,MAX_WRITEBUFFERLEN,TYPENUM_SCHEDULE))
            {
                (header->part_number)++;
                _oz_the_file.cur_partoffset=sizeof(*header);
                return didwrite;
            }
            (header->part_number)++;
            _oz_the_file.cur_partoffset=sizeof(*header);
        }
        curwrite=length;
        if(length>MAX_WRITEBUFFERLEN-(_oz_the_file.cur_partoffset))
          curwrite=MAX_WRITEBUFFERLEN-(_oz_the_file.cur_partoffset);
        memcpy((_oz_the_file.buffer)+(_oz_the_file.cur_partoffset),data,curwrite);
        (_oz_the_file.cur_partoffset)+=curwrite;
        length-=curwrite;
        data=(void*)(((char*)data)+curwrite);
        didwrite+=curwrite;
        _oz_the_file.curpos+=curwrite;
    }
    return didwrite;
}

