#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4

enum ozfile_errornumbers _ozerrno;
ozfile_t _ozfilelist[MAX_FILES];
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];
static byte init=0;

unsigned __ozloadpart(ozfile_t *curf,unsigned part_number);
byte ozflashread(unsigned p, unsigned a); int unlink(char *name);
byte _ozkill(unsigned file_id);

/* random number routines lifted from Hi-Tech C library */

static long    __seedrandx = 1;
static byte random_init=0;

static void initfiles(void)
{
    static int i;
    init=1;
    for(i=0;i<MAX_FILES;i++) _ozfilelist[i].buffer=NULL;
    atexit(closeall);
}

static utoa2digit(char *s, unsigned u)
{
    s[0]=(u/10)+'0';
    s[1]=(u%10)+'0';
}     

static void encode(cfile_header_t *_p, unsigned owner_id, unsigned _length)
{
    register cfile_header_t *p=_p;
    static unsigned length;
    length=_length;
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

int close(int handle)
{
    register ozfile_t *curf;
    static cfile_header_t *header;
    static int rv;
    if(handle>=MAX_FILES || handle<0) return -1;
    curf=_ozfilelist+handle;
    if(curf->mode & O_RDONLY)
    {
        free(curf->buffer);
        curf->buffer=NULL;
        return 0;
    }
    else if(curf->mode & O_WRONLY)
    {
        rv=0;
        header=(cfile_header_t *)(curf->buffer);
        encode(header,_ozfileowner,curf->cur_partoffset);
        header->last_part=1;
        if(ozwritefile(curf->buffer,curf->cur_partoffset,TYPENUM_SCHEDULE))
           rv=-1;
        free(curf->buffer);
        curf->buffer=NULL;
        return rv;
    }
    else return -1;
}

void closeall(void)
{
    static int i;
	for(i=0;i<MAX_FILES;i++)
        if(NULL!=_ozfilelist[i].buffer) close(i);
}


static void __readfirstcluster(ozfile_t *curf, unsigned loc, char *firstcluster)
{
    register cfile_header_t *header;
    header=(cfile_header_t *)(2+firstcluster);
    memcpy(curf->buffer,firstcluster+2,62);
    if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
        curf->last_cluster_of_part=1;
    else curf->last_cluster_of_part=0;
    curf->last_part=header->last_part;
    curf->owner_id=__ozfileheaderdecode(&(curf->part_length));
    curf->first_part_length=curf->part_length;
    curf->file_id=header->file_id;
    curf->cur_clusteroffset=curf->cur_partoffset=sizeof(*header);
    curf->loc=curf->startloc=loc;
    curf->mode=O_RDONLY;
    curf->cur_part=0;
    curf->curpos=0;
}

static int extant(unsigned id)
{
    static int i;
    for(i=0;i<MAX_FILES;i++)
        if(_ozfilelist[i].buffer != NULL
         && _ozfilelist[i].mode == O_WRONLY
         && _ozfilelist[i].file_id == id) return 1;
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

int open(char *_name, int mode)
{
    static cfile_header_t *header;
	static char *firstcluster;
    char name[MAX_FILENAMELEN+1];
	register ozfile_t *curf;
	static int handle;
	static unsigned loc,page,offset,recnum;
    static unsigned id,found_id;
    static int i;
	page=offset=0;
    if(!init)
        initfiles();
    strncpy(name,_name,MAX_FILENAMELEN);
    name[MAX_FILENAMELEN]=0;
	for(handle=0;handle<MAX_FILES;handle++)
     if(_ozfilelist[handle].buffer==NULL) break;
    if(handle==MAX_FILES && !(mode & O_CHECKONLY)) return -1;
    curf=&(_ozfilelist[handle]);
    if(!(mode & O_CHECKONLY))
      for(i=0;i<MAX_FILES;i++)
        if(_ozfilelist[i].buffer!=NULL
          && !strcmp(_ozfilelist[i].filename,name)) return -1;
    if((mode & O_RDONLY) || (mode & O_CHECKONLY))
	{
        while(loc=ozfindfilename(name,&page,&offset,&recnum))
		{
            if(mode & O_CHECKONLY) return 0;
/*            firstcluster=ozloadcluster(loc);
             header=(cfile_header_t *)(2+firstcluster); */
            if(NULL==(curf->buffer=malloc(62)))
            {
                _ozerrno=OUT_OF_MEMORY;
                return -1;
            }
            curf->mode=O_RDONLY;
            __readfirstcluster(curf,loc,ozloadcluster(loc));
			return handle;
		}
        _ozerrno=FILE_NOT_FOUND;
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
        curf->file_id=id;
        if(NULL==(curf->buffer=malloc(MAX_WRITEBUFFERLEN)))
            {
                _ozerrno=OUT_OF_MEMORY;
                return -1;
            }
		curf->cur_part=0;
        curf->curpos=0;
        curf->mode=O_WRONLY;
        for(i=0;i<62;i++) (curf->buffer)[i]=0;
        header=(cfile_header_t *)(curf->buffer);
        strcpy(header->fake_time,"1200");
/*        header->part_number=0; */
        header->header_id='c';
        header->file_id=curf->file_id;
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
        curf->cur_partoffset=sizeof(*header);
        return handle;
	}
}

int access(char *name)
{
    return open(name,O_CHECKONLY);
}

int rewind(int handle)
{
    register ozfile_t *curf;
#ifdef CLEANUP
    extern byte _ozfsdirty;
#endif
    if(handle>=MAX_FILES || handle<0) return -1;
    curf=_ozfilelist+handle;
    if(!(curf->mode & O_RDONLY)) return -1;
#ifdef CLEANUP
    if(!_ozfsdirty)
#endif
        __readfirstcluster(curf,curf->startloc,ozloadcluster(curf->startloc));
#ifdef CLEANUP
    else
    {
        static unsigned page,offset,recnum,loc;
        static cfile_header_t *header;
        static char *firstcluster;
        page=offset=0;
        while((loc=ozfindfileid(curf->file_id,&page,&offset,&recnum)))
        {
            header=(void*)(2+(firstcluster=ozloadcluster(loc)));
            if(header->part_number) continue;
            __readfirstcluster(curf,loc,firstcluster);
            break;
        }
        if(!loc) return -1;
    }
#endif
    return 0;
}

int __read(int handle, char *data, int length, int skip)
{
    static int didread;
    static int curread;
    static char *firstcluster;
    register ozfile_t *curf;
    if(handle>=MAX_FILES || handle<0) return -1;
    curf=_ozfilelist+handle;
    if(NULL==curf->buffer || curf->mode != O_RDONLY) return -1;
    didread=0;
    while(length>0)
    {
        if((curf->cur_partoffset) >= (curf->part_length))
		{
            if((curf->last_part && curf->last_cluster_of_part)
            || !__ozloadpart(curf,curf->cur_part+1))
            {
                return didread;
            }
        }
        if((curf->cur_clusteroffset)>=MAX_READBUFFERLEN)
		{
            if(curf->last_cluster_of_part)
            {
                if(curf->last_part) return didread;
                  else continue; /* This shouldn't happen */
            }
			curf->loc=ozinclocation(curf->loc);
			firstcluster=ozloadcluster(curf->loc);
			if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
				curf->last_cluster_of_part=1;
            else curf->last_cluster_of_part=0;
			memcpy(curf->buffer,firstcluster+2,62);
            curf->cur_clusteroffset=0;
		}
        curread=length;
        if(curread>MAX_READBUFFERLEN-(curf->cur_clusteroffset))
          curread=MAX_READBUFFERLEN-(curf->cur_clusteroffset);
        if(curread>(curf->part_length)-(curf->cur_partoffset))
          curread=(curf->part_length)-(curf->cur_partoffset);
        if(!skip) memcpy(data,curf->cur_clusteroffset+curf->buffer,curread);
        curf->cur_clusteroffset+=curread;
        curf->cur_partoffset+=curread;
		length-=curread;
		data+=curread;
		didread+=curread;
        curf->curpos+=curread;
    }
	return didread;
}

static int cleanupfiles(void)
{
    extern byte _ozfsdirty;
#ifdef CLEANUP
    register ozfile_t *curf;
    static int error;
    static int i;
    static long pos;
    error=0;
    for(i=0;i<MAX_FILES;i++)
    {
        curf=_ozfilelist+i;
        if(NULL!=curf->buffer && O_RDONLY==curf->mode)
        {
            pos=curf->curpos;
            if(rewind(i)) error=-1;
            if(pos!=lseek(i,pos,SEEK_SET)) error=-1;
        }
    }
#endif
    _ozfsdirty=0;
#ifdef CLEANUP
    return error;
#else
    return 0;
#endif
}

int read(int handle, void *data, int length)
{
    extern byte _ozfsdirty;
#ifdef CLEANUP
    if(_ozfsdirty && cleanupfiles()) return -1;
#endif
    return __read(handle,(char*)data,length,0);
}

int write(int handle, void *data, int length)
{
    static int didwrite;
    static int curwrite;
    register ozfile_t *curf;
    static cfile_header_t *header;
	if(handle>=MAX_FILES || handle<0) return -1;
    curf=_ozfilelist+handle;
    if(NULL==curf->buffer || curf->mode != O_WRONLY) return -1;
    didwrite=0;
	while(length>0)
    {
        if(curf->cur_partoffset >= MAX_WRITEBUFFERLEN)
        {
            header=(cfile_header_t *)(curf->buffer);
            encode(header,_ozfileowner,MAX_WRITEBUFFERLEN);
            header->last_part=0; /* not last */
            if(ozwritefile(curf->buffer,MAX_WRITEBUFFERLEN,TYPENUM_SCHEDULE))
            {
                (header->part_number)++;
                curf->cur_partoffset=sizeof(*header);
                return didwrite;
            }
            (header->part_number)++;
            curf->cur_partoffset=sizeof(*header);
        }
        curwrite=length;
        if(length>MAX_WRITEBUFFERLEN-(curf->cur_partoffset))
          curwrite=MAX_WRITEBUFFERLEN-(curf->cur_partoffset);
        memcpy((curf->buffer)+(curf->cur_partoffset),data,curwrite);
        (curf->cur_partoffset)+=curwrite;
        length-=curwrite;
        data=(void*)(((char*)data)+curwrite);
        didwrite+=curwrite;
        curf->curpos+=curwrite;
    }
    return didwrite;
}

