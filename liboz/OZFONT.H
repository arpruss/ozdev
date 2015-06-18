#ifndef _OZFONT_H
#define _OZFONT_H

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

/* 12 bytes long */
struct ozfontheader
{
    unsigned checksum;
    unsigned magic; /* 0x466F */
    unsigned length; /* including length of header */
    byte first;
    byte last;
    byte bitmap_height;
    byte line_height; /* recommended */
    byte maxwidth;
    byte active;
};

struct ozfonttableentry
{
    byte width;
    unsigned offset;
};

extern byte *_ozfontpointers[];
extern byte ozcustomactivefont;
byte ozscancustomfonts(void);
void ozsetfontpointer(byte *font);
void ozclearcustomfontslot(byte slot);
void ozwritecustomfontbyte(byte slot,unsigned offset,byte value);
void ozwritecustomfontmem(byte slot,byte *fontdata);
#define MASK_CUSTOM0 1
#define MASK_CUSTOM1 2
#define FONT_CUSTOM0 4
#define FONT_CUSTOM1 5
#define FONT_TEMPORARY 6
#define MAX_CUSTOM_SLOT_SIZE 0x0D80


#endif
