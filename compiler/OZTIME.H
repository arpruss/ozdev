/* 4. TIME */

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

/* some stuff for backwards compatibility -- deprecated, do not use */
#define ozsec1() (ozsec()%10)
#define ozsec2() (ozsec()/10)
#define ozmin1() (ozmin()%10)
#define ozmin2() (ozmin()/10)

unsigned ozsec(void);
unsigned ozmin(void);
unsigned ozhour(void);
unsigned ozweekday(void);
unsigned ozmonth(void);
unsigned ozday(void);
unsigned ozyear(void);
unsigned long oztime(void);

#ifndef NULL
#define NULL ((void*)0)
#endif


