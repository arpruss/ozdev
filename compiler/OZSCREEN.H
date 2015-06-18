/* 3. BACKLIGHT AND SCREEN CONTROL */
#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif


void oztogglelight(void);
void ozsetlight(byte l);
int ozgetlight(void);
void ozblankscreen(void);
void ozunblankscreen(void);
byte ozgetcontrast(void);
void ozsetcontrast(byte c);
#define MAX_CONTRAST 0x3F

#ifndef NULL
#define NULL ((void*)0)
#endif

