#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "file.h"

#define O_CHECKONLY 4

static char _buffer[MAX_READBUFFERLEN];

ozfile_t _oz_the_file_ro;
extern unsigned _ozfileowner;
extern unsigned _ozcdirpages[];

void _ozro1readfirstcluster(unsigned loc, char *firstcluster);
byte ozflashread(unsigned p, unsigned a);
int unlink(char *name);
byte _ozkill(unsigned file_id);

/* random number routines lifted from Hi-Tech C library */

int close1ro()
{
    if(_oz_the_file_ro.mode & O_RDONLY)
    {
        _oz_the_file_ro.buffer=NULL;
        return 0;
    }
    else return -1;
}

static void __ozreadnotfirstpart1(unsigned loc, char
*firstcluster)
{
    register cfile_header_t *header;
    header=(cfile_header_t *)(2+firstcluster);
    memcpy(_oz_the_file_ro.buffer,header,62);
    if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
      _oz_the_file_ro.last_cluster_of_part=1;
    else
      _oz_the_file_ro.last_cluster_of_part=0;
    _oz_the_file_ro.cur_part=header->part_number;
    _oz_the_file_ro.last_part=header->last_part;
    _oz_the_file_ro.cur_clusteroffset=_oz_the_file_ro.cur_partoffset=sizeof(*header);
    __ozfileheaderdecode(&(_oz_the_file_ro.part_length));
    _oz_the_file_ro.loc=loc;
}

unsigned __ozloadpart1ro(unsigned part_number)
{
    static unsigned loc;
    static unsigned page,offset,recnum;
    static cfile_header_t *header;
    register char *firstcluster;
    page=offset=0;
    while((loc=ozfindfileid(_oz_the_file_ro.file_id,&page,&offset,&recnum)))
    {
        header=(void*)(2+(firstcluster=ozloadcluster(loc)));
        if(header->part_number != part_number)
           continue;
        __ozreadnotfirstpart1(loc,firstcluster);
        break;
    }
    return loc;
}


void _ozro1readfirstcluster(unsigned loc, char *firstcluster)
{
    register cfile_header_t *header;
    header=(cfile_header_t *)(2+firstcluster);
    memcpy(_oz_the_file_ro.buffer,firstcluster+2,62);
    if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
        _oz_the_file_ro.last_cluster_of_part=1;
    else _oz_the_file_ro.last_cluster_of_part=0;
    _oz_the_file_ro.last_part=header->last_part;
    _oz_the_file_ro.owner_id=__ozfileheaderdecode(&(_oz_the_file_ro.part_length));
    _oz_the_file_ro.first_part_length=_oz_the_file_ro.part_length;
    _oz_the_file_ro.file_id=header->file_id;
    _oz_the_file_ro.cur_clusteroffset=_oz_the_file_ro.cur_partoffset=sizeof(*header);
    _oz_the_file_ro.loc=_oz_the_file_ro.startloc=loc;
    _oz_the_file_ro.mode=O_RDONLY;
    _oz_the_file_ro.cur_part=0;
    _oz_the_file_ro.curpos=0;
}

int open1ro(char *_name, int mode)
{
    static char name[MAX_FILENAMELEN+1];
	static unsigned loc,page,offset,recnum;
	page=offset=0;
    strncpy(name,_name,MAX_FILENAMELEN);
    name[MAX_FILENAMELEN]=0;
    if(_oz_the_file_ro.buffer!=NULL) return -1;
    if(!(mode & O_CHECKONLY))
        if(_oz_the_file_ro.buffer!=NULL
          && !strcmp(_oz_the_file_ro.filename,name)) return -1;
    if((mode & O_RDONLY) || (mode & O_CHECKONLY))
	{
        while(loc=ozfindfilename(name,&page,&offset,&recnum))
		{
            if(mode & O_CHECKONLY) return 0;
            if(NULL==(_oz_the_file_ro.buffer=_buffer))
            {
                return -1;
            }
            _oz_the_file_ro.mode=O_RDONLY;
            _ozro1readfirstcluster(loc,ozloadcluster(loc));
            return 0;
		}
		return -1;
	}
}

int __read1ro(char *data, int length, int skip)
{
    static int didread;
    static int curread;
    register char *firstcluster;
    if(NULL==_oz_the_file_ro.buffer || _oz_the_file_ro.mode != O_RDONLY) return -1;
    didread=0;
    while(length>0)
    {
        if((_oz_the_file_ro.cur_partoffset) >= (_oz_the_file_ro.part_length))
		{
            if((_oz_the_file_ro.last_part && _oz_the_file_ro.last_cluster_of_part)
            || !__ozloadpart1ro(_oz_the_file_ro.cur_part+1))
            {
                return didread;
            }
        }
        if((_oz_the_file_ro.cur_clusteroffset)>=MAX_READBUFFERLEN)
		{
            if(_oz_the_file_ro.last_cluster_of_part)
            {
                if(_oz_the_file_ro.last_part) return didread;
                  else continue; /* This shouldn't happen */
            }
            _oz_the_file_ro.loc=ozinclocation(_oz_the_file_ro.loc);
            firstcluster=ozloadcluster(_oz_the_file_ro.loc);
			if(*firstcluster=='\xFF' && firstcluster[1]=='\xFF')
                _oz_the_file_ro.last_cluster_of_part=1;
            else _oz_the_file_ro.last_cluster_of_part=0;
            memcpy(_oz_the_file_ro.buffer,firstcluster+2,62);
            _oz_the_file_ro.cur_clusteroffset=0;
		}
        curread=length;
        if(curread>MAX_READBUFFERLEN-(_oz_the_file_ro.cur_clusteroffset))
          curread=MAX_READBUFFERLEN-(_oz_the_file_ro.cur_clusteroffset);
        if(curread>(_oz_the_file_ro.part_length)-(_oz_the_file_ro.cur_partoffset))
          curread=(_oz_the_file_ro.part_length)-(_oz_the_file_ro.cur_partoffset);
        if(!skip) memcpy(data,_oz_the_file_ro.cur_clusteroffset+_oz_the_file_ro.buffer,curread);
        _oz_the_file_ro.cur_clusteroffset+=curread;
        _oz_the_file_ro.cur_partoffset+=curread;
		length-=curread;
		data+=curread;
		didread+=curread;
        _oz_the_file_ro.curpos+=curread;
    }
	return didread;
}

int read1ro(void *data, int length)
{
    return __read1ro((char*)data,length,0);
}
