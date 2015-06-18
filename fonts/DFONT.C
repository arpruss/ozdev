#define MAX_FONT_LEN 3456
#define FONT_HEADER 12
#define WIDTH_TABLE (128*3)
#define FONT_DATA (MAX_FONT_LEN-FONT_HEADER-WIDTH_TABLE)

int manualwidths[128];
unsigned char font_data[FONT_DATA];
unsigned char the_font[MAX_FONT_LEN];
unsigned offsets[128];
int offset;
int widths[128];
int maxwidth;
int rev=0;
int ltrim=0;
int dump=0;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ascii2me.c"

void err(char *s, ...)
{
#ifndef DLL
    va_list argptr;
    va_start(argptr,s);
    vfprintf(stderr,s,argptr);
    va_end(argptr);
#endif
#ifdef DLL
    longjmp(ExitJump,2);
#else
    exit(1);
#endif
}



/** 2/13/2001 Andrew Shakinovsky **/

#define BI_RGB		   0
#define BI_RLE8		1
#define BI_RLE4		2
#define BI_BITFIELDS	3

int font_width;
int font_height;
int bytesacross; /* number of bytes wide */
int bytesinrow;  /* number of bytes to a row (may be more than above due to padding */

/* doesn't include magic # */
typedef struct bmp_header_ {
	unsigned Size;
	unsigned short Reserved1;
	unsigned short Reserved2;
	unsigned OffBits;
} bmp_header;


typedef struct bmp_info_header_ {
	unsigned Size;
	int Width;
	int Height;
	unsigned short Planes;
	unsigned short BitCount;
	unsigned Compression;
	unsigned SizeImage;
	int XPelsPerMeter;
	int YPelsPerMeter;
	unsigned ClrUsed;
	unsigned ClrImportant;
} bmp_info_header;

typedef struct rgb_quad_ {
	char blue;
	char green;
	char red;
	char unused;
} rgb_quad;

char *font_bits; /* holds the whole image in memory */
char errormsg[255];

int process_file();

void flop(char *buf, unsigned len)
{
    char bitrev[256];
    unsigned i,j;
    for(i=0;i<256;i++)
    {
        bitrev[i]=0;
        for(j=0;j<8;j++)
            if(i & (1<<j)) bitrev[i]|=(1<<(7-j));
    }
    for(i=0;i<len;i++) buf[i]=~bitrev[(unsigned char)(buf[i])];
}


int process_file(FILE *in)
{
	bmp_header hdr;
	bmp_info_header infohdr;
	rgb_quad quad;

    char mask;

	int whiteindex;  /* set to 0 or 1 indicating which index is black in the color table */

	char *colbuf;

	int i, j, k;
	char magic[2];

	int colpos; /* current column pos */
    unsigned scratchtable[256];
	char slug;	/* current slug char */
	char cur;
	char next;
	int thisrun;


	if(fread(magic, 2,1,in) != 1) {
		strcpy(errormsg,"Error reading BMP magic #");
		return 1;
  	}

	if(strncmp(magic,"BM",2)) {
		strcpy(errormsg,"Not BMP format");
		return 1;
  	}

	if(fread(&hdr, sizeof(bmp_header),1,in) != 1) {
		strcpy(errormsg,"Error reading BMP header");
		return 1;
  	}

	if(fread(&infohdr, sizeof(bmp_info_header),1,in) != 1) {
		strcpy(errormsg,"Error reading BMP info header");
		return 1;
  	}

	if ( infohdr.Size == 12 ) {
		infohdr.Compression	= BI_RGB;
		infohdr.SizeImage	= 0;
		infohdr.XPelsPerMeter	= 0;
		infohdr.YPelsPerMeter	= 0;
		infohdr.ClrUsed	= 0;
		infohdr.ClrImportant	= 0;
  	}

	if(infohdr.BitCount != 1) {
		strcpy(errormsg,"Only 1bit BMP files are supported");
		return 1;
  	}

	for(i=0;i<2;i++) {
		if(fread(&quad, sizeof(rgb_quad),1,in) != 1) {
			strcpy(errormsg,"Error reading BMP info header");
			return 1;
		}
		if(quad.blue) whiteindex=i; /* they all will have a value for white, so pick blue */
	}

	bytesacross = (infohdr.Width + 7)/8;
	bytesinrow = infohdr.SizeImage/infohdr.Height;
    font_width=infohdr.Width;
    font_height=infohdr.Height;

    font_bits=(char *)malloc(infohdr.SizeImage);
    if(font_bits==NULL) err("Out of memory!\n");

    if(fread(font_bits,infohdr.SizeImage,1,in) != 1) {
		strcpy(errormsg,"Bitmap file corrupt");
        free(font_bits);
		return 1;
  	}

    flop(font_bits,infohdr.SizeImage);
    if(infohdr.Width % 8)
    {
        mask=0;
        for(i=0;i<infohdr.Width % 8;i++)
          mask=(mask<<1) | 1;
        for(j=0;j<infohdr.Height;j++)
          font_bits[(j*bytesinrow)+bytesacross-1] &= mask;
    }
    return 0;
}

/* rest of code by ARP */

int limit_width;

FILE *binfile;

int pos;
unsigned short checksum;

void writebyte(unsigned char c)
{
    fputc(c,binfile);
}

void dumpbyte(unsigned char c)
{
    the_font[pos++]=c;
    checksum+=c;
}

void dumpword(unsigned int c)
{
    dumpbyte(c%256);
    dumpbyte(c/256);
}

int getpixel(int x,int y)
{
    if(x<0 || y<0 || x>=font_width || y>=font_height) return 0;
    else return rev^(1&(((unsigned char)font_bits[(x/8)+(font_height-1-y)*bytesinrow])>>(x%8)));
}

void addbyte(unsigned char c)
{
    if(offset>=FONT_DATA) err("Font too big!\n");
    font_data[offset++]=c;
}

void setwidth(int c,int w)
{
    if(w>maxwidth) maxwidth=w;
    widths[c]=w;
}

void addcr(int h)
{
    int i;
    setwidth(13,5);
    offsets[13]=offset;
    i=0;
    if(h<3) return;
    if(h>6)
    {
        addbyte(0);
        i++;
    }
    for(;i<h-3;i++)
        addbyte(0x08);
    addbyte(0x0a); /* h-3 */
    addbyte(0x0f); /* h-2 */
    addbyte(0x02); /* h-1 */
    addbyte(0);
}

int height_above;
int height_below;

int have_underline(int x0,int y0,int underline_width)
{
    int x;
    int y;
    int i;
    if(x0+underline_width>font_width) return 0;
    for(x=x0;x<x0+underline_width;x++) if(!getpixel(x,y0)) return 0;
    for(i=1;i<height_above;i++)
    {
        y=y0-i;
        for(x=x0;x<x0+underline_width;x++) if(getpixel(x,y)) return 0;
    }
    for(i=1;i<height_below;i++)
    {
        y=y0+i;
        for(x=x0;x<x0+underline_width;x++) if(getpixel(x,y)) return 0;
    }
    return 1;
}

int have_underline_strict(int x0,int y0,int underline_width)
{
    int x,y;
    /* do we have an underline followed by a blank */
    if(!have_underline(x0,y0,underline_width)) return 0;
    for(x=x0+underline_width;x<x0+underline_width+2;x++)
        for(y=y0;y>y0-height_above;y--)
            if(getpixel(x,y)) return 0;
    return 1;
}

void grab_char(int c,int x0,int y0,int width,int height)
{
    int x,y;
    unsigned int value;
    width-=ltrim;
    x0+=ltrim;
    if(width>8 && limit_width) width=8;
    setwidth(c,(width<2)?2:width);
    offsets[c]=offset;
    if(dump)
    {
        printf("%c : width=%d : at %d,%d\n",c,width,x0,y0);
    }
    for(y=y0;y<y0+height;y++)
    {
        value=0;
        for(x=x0;x<x0+width;x++)
        {
            value=(value>>1)|(getpixel(x,y)<<(width-1));
            if(dump) putchar(getpixel(x,y)?'X':'.');
        }
        if(dump) putchar('\n');
        if(width>8)
        {
          addbyte(value%256);
          addbyte(value/256);
        }
        else addbyte(value);
    }
}

int process_char(char c,int x0,int y0,int height,int underline_height,int underline_width)
{
    int x,y;
    y=y0+underline_height;
    x=x0;
    if(!have_underline(x,y,underline_width)) return -1;
    x+=underline_width;
    x0+=underline_width;
    if(manualwidths[c]>=0)
        x=x0+manualwidths[c];
    else
        for(;x<font_width && !have_underline_strict(x,y,underline_width);x++);
    if(x-x0>16 || x>=font_width) err("Too wide char %c (width %d at %d,%d)!",c,x-x0,x,y0);
    grab_char(c,x0,y0,x-x0,height);
    for(x++;x<font_width && !have_underline(x,y,underline_width);x++);
    return x;
}

int clearline(int y)
{
    int x;
    for(x=0;x<font_width;x++) if(getpixel(x,y)) return 0;
    return 1;
}

int findnext(int x,int y,int uw)
{
    for(y++;y<font_height;y++)
        if(have_underline(x,y,uw)) break;
    return y;
}


main(int argc, char **argv)
{
    FILE *f;
    char name[512];
    int underline_width;
    int height;
    int real_height;
    int x,y;
    int x0,y0;
    int c;
    int i;
    int length;

    for(i=0;i<128;i++) manualwidths[i]=-1;

    if(argc<2) err("dfont font [l#] [d] [mc#] [mc#] ...\n"
                    "  You must have font.bmp on your system in the right format.\n");
    if(argc>2)
    {
        for(i=2;i<argc;i++)
        {
            switch(argv[i][0])
            {
                case 'l':
                    ltrim=atoi(argv[i]+1);
                    break;
                case 'd':
                    dump=1;
                    break;
                case 'm':
                    manualwidths[argv[i][1]]=atoi(argv[i]+2);
                    break;
                default:
                    err("dfont: unknown option\n");
            }
        }
    }
    strcpy(name,argv[1]);
    strcat(name,".bmp");
    f=fopen(name,"rb");
    if(f==NULL) err("Cannot open %s.\n",argv[1]);
    if(process_file(f)) err(errormsg);
    fclose(f);
    strcpy(name,argv[1]);
    strcat(name,".wzf");
    binfile=fopen(name,"wb");
    if(binfile==NULL) err("Cannot open %s.\n",argv[1]);

    for(i=0;i<128;i++) widths[i]=-1;

/*    if(argc>1) limit_width=0;
     else limit_width=1;
*/
    limit_width=0;

    rev=getpixel(0,0);
    for(x=0;x<font_width;x++)
    {
        for(y=0;y<font_height;y++)
        {
            if(getpixel(x,y)) goto OK;
        }
    }
    if(x==font_width) err("Cannot find start!");
OK:
    x0=x;
    y0=y;
    for(x=x0+1;x<font_width && getpixel(x,y);x++);
    if((underline_width=x-x0)>16 || x>font_width) err("Underline too wide (%d)!",underline_width);
    for(y=y0-1;y>=0 && !clearline(y);y--);
    if(y<0) y=0;
    height_above=y0-y-1;
    for(y=y0+1;y<font_height && !clearline(y);y++);
    if(y==font_height) err("No blank line after first line!");
    height_below=y-y0-1;
    for(y=y0+1;y<font_height && !have_underline(x0,y,underline_width);y++);
    if(y==font_height) err("No second line!");
    height=(y-y0)/2;
    x=x0;
    printf("myfont_height=%d\n"
           "underline_width=%d\n"
           "height_above=%d\n"
           "height_below=%d\n"
           "line_spacing=%d\n",
           real_height=height_above+height_below+1,underline_width,height_above,height_below,
           height);
    maxwidth=offset=0;
    addcr(real_height);
    c=32;
    while(c<127 && y<font_height)
    {
        if((x=process_char(c,x,y-height_above,real_height,height_above,underline_width))<0)
        {
            x=x0;
            y=findnext(x,y,underline_width);
        }
        else
        {
            c++;
            if(x>=font_width)
            {
                x=x0;
                y=findnext(x,y,underline_width);
            }
        }
    }
    if(c!=127) err("Not enough characters!");

    checksum=pos=0;
    dumpword(0); /* checksum */
    dumpword(0x466F); /* magic */
    dumpword(length=offset+WIDTH_TABLE+FONT_HEADER); /* length */
    printf("length=%d\n",length);
    dumpbyte(0);
    dumpbyte(127);
    dumpbyte(real_height);
    dumpbyte(height); /* line height */
    dumpbyte(maxwidth);
    dumpbyte(0); /* active */
    for(i=0;i<128;i++)
    {
        if(widths[i]<0)
        {
            if(i<32 || i==127) c=(i==127)?'e':ascii2me[i];
              else c='*';
            }
        else c=i;
        dumpbyte(widths[c]);
        dumpword(offsets[c]+WIDTH_TABLE+FONT_HEADER);
    }
    for(i=0;i<offset;i++)
    {
        dumpbyte(font_data[i]);
    }
    pos=0;
    dumpword(checksum);
    pos=0;
    for(i=0;i<length;i++)
    {
        writebyte(the_font[i]);
    }
    fclose(binfile);
    return 0;
}
