/* 8. AUXILIARY MEMORY   [48K model only] */

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

unsigned ozgetauxsize(void);
void ozwriteauxmem(unsigned address,void *data,unsigned length);
void ozreadauxmem(unsigned address,void *data,unsigned length);
void ozwriteauxword(unsigned address,unsigned value);
void ozwriteauxbyte(unsigned address,byte value);
byte ozreadauxbyte(unsigned address);
unsigned ozreadauxword(unsigned address);
unsigned ozcrc16auxmem(unsigned address,unsigned length);

#ifndef NULL
#define NULL ((void*)0)
#endif

