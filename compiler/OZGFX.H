/* 1. GRAPHICS AND DISPLAY (in DOCS\LIBOZ.TXT) */

#ifndef OZ7XX
#define OZ7XX
#ifdef putchar
# undef putchar
int putchar(char c);
#endif
#endif

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

void ozsetactivepage(byte page);
void ozsetdisplaypage(byte page);
byte ozgetactivepage(void);
byte ozgetdisplaypage(void);
void ozdisplayactivepage(void);
void ozactivatedisplaypage(void);
void ozcopypage(byte dest, byte src);
void ozswapactivedisplaypages(void);
#define MAX_DISPLAYPAGES 2

void ozcls(void);
int _ozpoint(byte x, byte y, byte color);
int ozpoint(int x, int y, byte color);
void ozcircle(int x,int y,byte r,byte color);
void ozline(int x,int y,int x2,int y2,byte color);
void _ozline(byte x,byte y,byte x2,byte y2,byte color);
void _ozhline(byte x,byte y,byte len,byte color);
void _ozvline(byte x,byte y,byte len,byte color);
void ozdisplayorbyte(unsigned offset, byte v);
void ozdisplayputbyte(unsigned offset, byte v);
void ozdisplayandbyte(unsigned offset, byte v);
byte ozdisplayinbyte(unsigned offset);
int ozgetpoint(int x, int y);
void _ozbox(byte x, byte y, byte width, byte height);
void ozbox(int x, int y, int width, int height);
int ozsetgreyscale(byte grey);
byte ozgetgreyscale(void);
void ozgreyfilledcircle(int x,int y,byte r,byte shade);
void ozgreycircle(int x,int y,byte r,byte shade);
void ozgreyline(int x1,int y1,int x2,int y2,byte shade);
int ozgreypoint(int x1,int y1,byte shade);
byte ozgetfontheight(byte f);
int ozgreyputs(int x,int y,byte shade,char *s);
void ozgreycls(void);
void ozgreyfilledbox(int x,int y,int w,int h,byte shade);
void ozgreybox(int x,int y,int w,int h,byte shade);
int ozgreygetpoint(int x, int y);
int ozgreyeditline(byte x0,byte y0,char *s,byte slen,byte xlen,byte shade);
int ozgreyputch(int x,int y,byte shade,char c);
void ozfilledbox(int x,int y,int w,int h,byte color);
void _ozfilledbox(byte x,byte y,byte w,byte h,byte color);
void ozscroll(unsigned numbytes);
void ozscrolldown(unsigned numbytes);
void ozscrollclear(void);
void ozsavescreen(void);
void ozrestorescreen(void);
void _ozputsprite(byte x,byte y,byte height,byte *sprite);
char *ozputsgetend(void);
int ozputs_system(int x, int y, char *string);
int ozputs(int x, int y, char *string);
void ozfont(byte fontnum);
byte ozgetfont();
void ozsetfont(byte fontnum);
int ozputch(int x, int y, char c);
void ozscrollright(byte y , byte rows);
void ozscrollleft(byte y , byte rows);

#define GREYSHADE_WHITE 0
#define GREYSHADE_BLACK 3
#define GREYSHADE_GREY1 1
#define GREYSHADE_GREY2 2
#define WHITE 0
#define BLACK 1
#define XOR   2
#define FILL   4
#define UNFILL 0
#define FONT_PC_NORMAL 1
#define FONT_PC_LARGE 0
#define FONT_OZ_NORMAL 3
#define FONT_OZ_LARGE 2

#ifndef NULL
#define NULL ((void*)0)
#endif

