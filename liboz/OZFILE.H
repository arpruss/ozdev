/* 7. FILE I/O */

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

unsigned ozfindnext(char type, unsigned *start_page, unsigned
  *start_offset, unsigned *recnum);
char *ozloadcluster(unsigned loc);
unsigned ozinclocation(unsigned loc);
unsigned ozfindmemo(char *memo, unsigned *start_page, unsigned
  *start_offset, unsigned *recnum);
unsigned ozfindfileid(unsigned fileid, unsigned *start_page, unsigned
  *start_offset, unsigned *recnum);
unsigned ozfindfilename(char *filename, unsigned *start_page, unsigned
  *start_offset, unsigned *recnum);
unsigned ozunlink(byte typenum, unsigned recnum);
/* for ozfindnext */

/* These are third characters of the type identifiers, which, luckily,
are unique. */
#define TYPE_MEMO 'M'
#define TYPE_SCHEDULE 'C'
#define TYPE_SYSTEM 'Y'
#define TYPE_TELEPHONE 'E'
#define TYPE_EXPENSES 'P'
#define TYPE_BASIC 'I'
#define TYPE_ANNIVERSARY 'N'
#define TYPE_TODO 'D'

/* These are used by writing and deleting system functions */
#define TYPENUM_SYSTEM	  0
#define TYPENUM_TELEPHONE 1
#define TYPENUM_MEMO	  2
#define TYPENUM_SCHEDULE  3
#define TYPENUM_ANNIVERSARY 4
#define TYPENUM_TODO	  5
#define TYPENUM_EXPENSES  6
#define TYPENUM_BASIC	  7

#define O_RDONLY 1
#define O_WRONLY 2
#define MAX_FILENAMELEN 23
#define MAX_FILES 4
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


int close(int handle);
int close1(void);
int write(int handle, void *data, int length);
int write1(void *data, int length);
int write1ro(void *data, int length);
int read(int handle, void *data, int length);
int read1(void *data, int length);
int read1ro(void *data, int length);
int open(char *_name, int mode);
int open1(char *_name, int mode);
int open1ro(char *_name, int mode);
long lseek(int handle, long offset, int fromwhere);
long lseek1(long offset, int fromwhere);
long lseek1set(long offset); long lseek1ro(long offset, int fromwhere);
long tell1(void);
int access(char *name); int unlink(char *name); int
ozunlinkallowned(void); void closeall(void); void ozsetowner(unsigned
o); unsigned ozgetowner(void);

struct appindexlist
{
	unsigned unknown;
	unsigned indexes[3];
};

void ozgetappindexlist(byte typenum, struct appindexlist *ilist);
extern byte _ozindexbycounts[8];
byte ozgetindexby(byte typenum);
void ozsetindexby(byte typenum,byte value);
int ozwritefile(void *data, unsigned length, byte type);
int ozopenfile(unsigned loc);
int ozreadfilebyte(void);
int ozopenmemo(char *title);
void ozclosefile(void);
unsigned ozgetlastrecnum(void);
unsigned ozfindrecnum(unsigned recnum);

#define MAX_MEMOLEN 1956

#ifndef NULL
#define NULL ((void*)0)
#endif

