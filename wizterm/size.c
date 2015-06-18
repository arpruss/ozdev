#include <oz.h>

#define JR 0x18

extern byte width,height,charheight;
extern byte the_width;
extern unsigned word_180_150,word_179_149,word_150_120;
extern unsigned word_180_150_2,word_179_149_2,word_150_120_2;
extern unsigned word_150_120_3;
extern unsigned word_2919h_1929h;
extern unsigned word_2919h_1929h_2;
extern byte byte_0_1 /* ,byte_0_15 */ ;
extern char nop_start[],nop_end[],nop_start_2[],nop_end_2[];
extern char nop_start_3[],nop_end_3[];
extern char nop_start_4[],nop_end_4[];
extern byte underline;

unsigned nop1, nop2, nop3, nop4;

void setcols(byte cols)
{
    the_width=width=cols;
}

void setlines16(void)
{
    if(height==16) return;
    height=16;
    charheight=5;
    nop1=*(unsigned *)(nop_start);
    nop2=*(unsigned *)(nop_start_2);
    nop3=*(unsigned *)(nop_start_3);
    nop4=*(unsigned *)(nop_start_4);
    nop_start[0]=nop_start_2[0]=nop_start_3[0]=nop_start_4[0]=JR;
    nop_start[1]=(byte)(nop_end-nop_start-2);
    nop_start_2[1]=(byte)(nop_end_2-nop_start_2-2);
    nop_start_3[1]=(byte)(nop_end_3-nop_start_3-2);
    nop_start_4[1]=(byte)(nop_end_4-nop_start_4-2);
    word_180_150=150;   word_179_149=149;   word_150_120=120;
    word_180_150_2=150; word_179_149_2=149; word_150_120_2=120;
    word_150_120_3=120;
    word_2919h_1929h=0x1929;
    word_2919h_1929h_2=0x1929;
    byte_0_1=1;
    underline=0;
/*    byte_0_15=15; */
    scrolling_region(1,height);
}

void setlines13(void)
{
    if(height==13) return;
    height=13;
    charheight=6;
    *(unsigned *)(nop_start)=nop1;
    *(unsigned *)(nop_start_2)=nop2;
    *(unsigned *)(nop_start_3)=nop3;
    *(unsigned *)(nop_start_4)=nop4;
    word_180_150=180;   word_179_149=179;   word_150_120=150;
    word_180_150_2=180; word_179_149_2=179; word_150_120_2=150;
    word_150_120_3=150;
    word_2919h_1929h=0x2919;
    word_2919h_1929h_2=0x2919;
    byte_0_1=0;
/*    byte_0_15=0; */
    scrolling_region(1,height);
}
