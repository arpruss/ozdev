#include <oz.h>
#include "edit.h"

#define TRANSL

#ifdef TRANSL
extern byte xlat_table[];
#endif

byte NUM_FONTS;
byte font2;
byte font3;


byte font_narrow_height;
byte line_height;
byte lines_per_screen;
unsigned line_height_bytes;

byte measurechar(char c)
{
    line_buf[0]=c;
    line_buf[1]=0;
    return ozputsf(-1);
}

byte measurestring(char *s)
{
    strcpy(line_buf,s);
    return ozputsf(-1);
}

void set_edit_font(byte n)
{
    extern byte xlat_override,do_grab_transl;
    static byte i;
    static byte f;
    switch(n)
    {
      case 1:
        xlat_override=0;
        do_grab_transl=32;
        f=FONT_PC_LARGE;
/*        font_narrow_height=line_height=13;
        line_height_bytes=13*30;
        lines_per_screen=80/13; */
        break;
      case 0:
        xlat_override=0;
        do_grab_transl=32;
        f=FONT_PC_NORMAL;
/*        font_narrow_height=line_height=8;
        line_height_bytes=8*30;
        lines_per_screen=80/8; */
        break;
      case 2:
        xlat_override=1;
        f=font2;
        break;
      case 3:
        xlat_override=1;
        f=font3;
        break;
    }
    ozsetappfont(f);
    ozsetfont(f);
    font_narrow_height=line_height=ozgetfontheight(f);
    line_height_bytes=(unsigned int)font_narrow_height*30;
    lines_per_screen=80/font_narrow_height;
    if(lines_per_screen>MAX_LINES_PER_SCREEN) lines_per_screen=MAX_LINES_PER_SCREEN;


#ifdef TRANSL
    i=32;
    do
    {
        width_table[i]=measurechar(i);
    }
    while(++i);
    for(i=0;i<32;i++)
    {
        width_table[i]=measurechar(n<2?xlat_table[i]:i);
    }
    if(n==0) width_table[13]=6;  /* CR fixup */
    if(n>1) width_table[0]=width_table[127];
#else
    for(i=0;i<128;i++)
      width_table[i]=measurechar(i);
#endif
}
