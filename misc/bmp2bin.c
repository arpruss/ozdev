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


int getpixel(int x,int y)
{
    if(x<0 || y<0 || x>=font_width || y>=font_height) return 0;
    else return rev^(1&(((unsigned char)font_bits[(x/8)+(font_height-1-y)*bytesinrow])>>(x%8)));
}


main(int argc, char **argv)
{
    FILE *f;
    FILE *binfile;
    char name[512];
    int x,y;
    int b;
    int i;
    int val;

    if(argc<2) err("bmp2bin font [r]\n");
    strcpy(name,argv[1]);
    strcat(name,".bmp");
    f=fopen(name,"rb");
    if(f==NULL) err("Cannot open %s.\n",argv[1]);
    if(process_file(f)) err(errormsg);
    fclose(f);
    strcpy(name,argv[1]);
    strcat(name,".bin");
    binfile=fopen(name,"wb");
    if(binfile==NULL) err("Cannot open %s.\n",argv[1]);
    rev=(argc==3);
    printf("Converting %d x %d image\n",font_width,font_height);
    for(y=0;y<font_height && y<80;y++)
    {
        for(b=0;b<30;b++)
        {
            val=0;
            for(i=0;i<8;i++)
                val+=getpixel(b*8+i,y)<<i;
            fputc(val,binfile);
        }
    }
    fcloseall();
    return 0;
}

