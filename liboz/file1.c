#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4

extern char _ozfile1buffer[MAX_WRITEBUFFERLEN];
#define _buffer _ozfile1buffer

ozfile_t _oz_the_file;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
static byte init=0;

extern char _ozclusterbuffer[];

#define header0 (*(cfile_header_t *)(2+_ozclusterbuffer))
#define header1 (*(cfile_header_t *)(_oz_the_file.buffer))

void _ozreadfirstcluster1(unsigned loc);
byte ozflashread(unsigned p, unsigned a);
int unlink(char *name);
byte _ozkill(unsigned file_id);

/* random number routines lifted from Hi-Tech C library */

static long    __seedrandx = 1;
static byte random_init=0;

int close1(void);

static void initfiles(void)
{
    init=1;
    _oz_the_file.buffer=NULL;
    atexit(close1);
}

static utoa2digit(char *s, unsigned u)
{
    s[0]=(u/10)+'0';
    s[1]=(u%10)+'0';
}     

static void encode(unsigned _owner_id, unsigned _length)
{
   static unsigned length,owner_id;
   length=_length;
   owner_id=_owner_id;

    /* 01234567 (fake_time) */
    /* HHMMHHMM */
    /* 0123 (fake_alarm) */
    /* HHMM */
   (header1.fake_time)[8]=(header1.fake_alarm)[4]=0;
   utoa2digit((header1.fake_alarm)+2,length%60);
   length/=60;
   utoa2digit(header1.fake_alarm,length%24);
   length/=24;
   (header1.fake_time)[1]=length+'0';  /* maximum length: 5759 */
   utoa2digit((header1.fake_time)+6,owner_id%60);
   owner_id/=60;
   utoa2digit((header1.fake_time)+4,14+owner_id%10);
   owner_id/=10;
   utoa2digit((header1.fake_time)+2,owner_id%60);
   owner_id/=60;
   /* now owner_id is either 0 or 1 */
   (header1.fake_time)[0]=owner_id+'0';
}

#define mysrand __ozfilemysrand
#define myrand __ozfilemyrand
int __ozfilemysrand(void)
{
    random_init=1;
    __seedrandx = ozsec()+60*ozmin()+3600l*ozhour()+86400l*ozday();
}

int __ozfilemyrand()
{
    return(((__seedrandx = __seedrandx*1103515245l + 12345l)>>16) &
    077777l);
}

int close1()
{
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
        encode(_ozfileowner,_oz_the_file.cur_partoffset);
        header1.last_part=1;
        if(ozwritefile(_oz_the_file.buffer,_oz_the_file.cur_partoffset,TYPENUM_SCHEDULE))
           rv=-1;
        _oz_the_file.buffer=NULL;
        return rv;
    }
}

static void __ozreadnotfirstpart1(unsigned loc)
{
    memcpy(_oz_the_file.buffer,&header0,62);
    if(*(unsigned *)_ozclusterbuffer==0xFFFF)
      _oz_the_file.last_cluster_of_part=1;
    else
      _oz_the_file.last_cluster_of_part=0;
    _oz_the_file.cur_part=header0.part_number;
    _oz_the_file.last_part=header0.last_part;
    _oz_the_file.cur_clusteroffset=_oz_the_file.cur_partoffset=sizeof(header0);
    __ozfileheaderdecode(&(_oz_the_file.part_length));
    _oz_the_file.loc=loc;
}

unsigned __ozloadpart1(unsigned part_number)
{
    static unsigned loc;
    static unsigned page,offset,recnum;
    page=offset=0;
    while((loc=ozfindfileid(_oz_the_file.file_id,&page,&offset,&recnum)))
    {
        ozloadcluster(loc);
        if(header0.part_number != part_number)
           continue;
        __ozreadnotfirstpart1(loc);
        break;
    }
    return loc;
}



void _ozreadfirstcluster1(unsigned loc)
{
    memcpy(_oz_the_file.buffer,&header0,62);
    if(*(unsigned*)_ozclusterbuffer==0xFFFF)
        _oz_the_file.last_cluster_of_part=1;
    else _oz_the_file.last_cluster_of_part=0;
    _oz_the_file.last_part=header0.last_part;
    _oz_the_file.owner_id=__ozfileheaderdecode(&(_oz_the_file.part_length));
    _oz_the_file.first_part_length=_oz_the_file.part_length;
    _oz_the_file.file_id=header0.file_id;
    _oz_the_file.cur_clusteroffset=_oz_the_file.cur_partoffset=sizeof(header0);
    _oz_the_file.loc=_oz_the_file.startloc=loc;
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

int open1(char *_name, int mode)
{
    static char name[MAX_FILENAMELEN+1];
	static unsigned loc,page,offset,recnum;
    static unsigned id,found_id;
    static byte i;
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
            if(NULL==(_oz_the_file.buffer=_buffer))
                return -1;
            _oz_the_file.mode=O_RDONLY;
            ozloadcluster(loc);
            _ozreadfirstcluster1(loc);
            return 0;
		}
		return -1;
	}
	else if(mode & O_WRONLY)  /* really, this should check O_CREAT */
	{
        found_id=0;
        while(loc=ozfindfilename(name,&page,&offset,&recnum))
        {
            ozloadcluster(loc);
            found_id=1;
            id=header0.file_id;
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
        strcpy(header1.fake_time,"1200");
/*        header1.part_number=0; */
        header1.header_id='c';
        header1.file_id=_oz_the_file.file_id;
        (header1).internal_file_id=0xFFFF;
        (header1).header_other[0]=0xD0;
        (header1).header_other[1]=0x0D;
/*        (header1).header_other[2]=0x00; */
        (header1).header_other[3]=0x16;
/*        (header1).header_other[4]=0x00; */
        (header1).header_other[5]=0x1F;
/*        (header1).header_other[6]=0x00; */
        strcpy(header1.fake_date,"19010101");
        strcpy(header1.filename,name);
        _oz_the_file.cur_partoffset=sizeof(header1);
        return 0;
	}
}

int __read1(char *_data, int length, int skip)
{
    static int didread;
    static int curread;
    register char *data;
    data=_data;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_RDONLY) return -1;
    didread=0;
    while(length>0)
    {
        if((_oz_the_file.cur_partoffset) >= (_oz_the_file.part_length))
		{
            if((_oz_the_file.last_part && _oz_the_file.last_cluster_of_part)
            || !__ozloadpart1(_oz_the_file.cur_part+1))
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
            ozloadcluster(_oz_the_file.loc);
            if(((unsigned*)_ozclusterbuffer)[0]==0xFFFF)
                _oz_the_file.last_cluster_of_part=1;
            else _oz_the_file.last_cluster_of_part=0;
            memcpy(_oz_the_file.buffer,&header0,62);
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

int read1(void *data, int length)
{
    return __read1((char*)data,length,0);
}

int write1(void *data, int length)
{
    static int didwrite;
    static int curwrite;
    if(NULL==_oz_the_file.buffer || _oz_the_file.mode != O_WRONLY) return -1;
    didwrite=0;
	while(length>0)
    {
        if(_oz_the_file.cur_partoffset >= MAX_WRITEBUFFERLEN)
        {
            encode(_ozfileowner,MAX_WRITEBUFFERLEN);
            header1.last_part=0; /* not last */
            if(ozwritefile(_oz_the_file.buffer,MAX_WRITEBUFFERLEN,TYPENUM_SCHEDULE))
            {
                (header1.part_number)++;
                _oz_the_file.cur_partoffset=sizeof(header1);
                return didwrite;
            }
            (header1.part_number)++;
            _oz_the_file.cur_partoffset=sizeof(header1);
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

