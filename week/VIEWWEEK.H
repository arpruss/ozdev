#define MAXAPPTS 115
#define NUMDAYS  7

#define PROP_FONT_HEIGHT 8
#define SCREEN_WIDTH 239
#define SCREEN_HEIGHT 80
#define NOHEAD_INDENT 18
#define TIME_INDENT   52

#define FIXED_FONT_HEIGHT 6

byte ozputch0(byte x,byte y,byte c);
unsigned mygetch(void);

#define PROPPUTCH0(c)  x=ozputch0(x,y,c)
#define PROPPUTCH(c)  x=ozputch(x,y,c)
#define PROPPUTS(c)   x=ozputs(x,y,c)

#define WIDTH  59
#define HEIGHT 13

/* #define ENGLISH /* ENGLISH or ITALIAN */
#define ENGLISH
