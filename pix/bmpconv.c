/** 2/13/2001 Andrew Shakinovsky **/
#include <stdio.h>

#define BI_RGB		   0
#define BI_RLE8		1
#define BI_RLE4		2
#define BI_BITFIELDS	3

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


FILE *in;
FILE *out;
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


int main(int argc, char *argv[])
{
	int ret;

	if(argc < 3) {
		fprintf(stderr,"Usage: %s <inputfile.bmp> <outputfile>\n", argv[0]);
		return 1;
  	}

	if(!(in=fopen(argv[1],"rb"))) {
		fprintf(stderr,"Error opening %s for input\n", argv[1]);
		return 1;
   }

   if(!(out=fopen(argv[2],"wb"))) {
		fprintf(stderr,"Error opening %s for output\n", argv[2]);
		return 1;
   }

	if(sizeof(bmp_header) != 12 || sizeof(bmp_info_header) != 40 ) {
		/* in case the compiler is not using 4/2/1 byte packing */
		fprintf(stderr,"Struct packing problem: header=%d s/b 12 info=%d s/b 40\n", sizeof(bmp_header), sizeof(bmp_info_header));
		return 1;
  	}

	ret=process_file();
	if(ret) {
		fprintf(stderr,"Error processing file: %s\n", errormsg);
  	}

	fclose(in);
	fclose(out);

	return ret;


}

void emit1(char val)
{
	fwrite(&val,1,1,out);
/*	printf("<%d>",(unsigned char)val);*/
}

void emit2(short val)
{
	fwrite(&val,2,1,out);
/*	printf("<_%d_>",val); */
}


int process_file()
{
	bmp_header hdr;
	bmp_info_header infohdr;
	rgb_quad quad;

    char mask;

	int bytesacross; /* number of bytes wide */
	int bytesinrow;  /* number of bytes to a row (may be more than above due to padding */
	int whiteindex;  /* set to 0 or 1 indicating which index is black in the color table */

	char *imagebuffer; /* holds the whole image in memory */
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

	imagebuffer=(char *)malloc(infohdr.SizeImage);

	if(fread(imagebuffer,infohdr.SizeImage,1,in) != 1) {
		strcpy(errormsg,"Bitmap file corrupt");
		free(imagebuffer);
		return 1;
  	}

    flop(imagebuffer,infohdr.SizeImage);
    if(infohdr.Width % 8)
    {
        mask=0;
        for(i=0;i<infohdr.Width % 8;i++)
          mask=(mask<<1) | 1;
        for(j=0;j<infohdr.Height;j++)
          imagebuffer[(j*bytesinrow)+bytesacross-1] &= mask;
    }

	colbuf=(char *)malloc(infohdr.Height);

	/* write the file header */
	emit1(169);
	emit2(infohdr.Height);
	emit2(bytesacross);

	/* scan each column */
	for(i=0;i<bytesacross;i++) { /* i is current column */

		/* find least occuring byte in column */
        memset(scratchtable,0,256*sizeof(*scratchtable));
		for(j=0;j<infohdr.Height;j++) {
			scratchtable[(unsigned char)imagebuffer[(j*bytesinrow)+i]] ++;
		}
		slug=0;
		for(j=0;j<256;j++) {
			if(scratchtable[j] < scratchtable[(int)slug]) slug=(char)j;
		}
		colpos=0;

		/* scan the column for runs (remember, bmp format is bottom to top) */
		thisrun=1;
		for(j=infohdr.Height-1;j >=0;j--) {
			cur=imagebuffer[(j*bytesinrow)+i];

			if(j) {
				next=imagebuffer[((j-1)*bytesinrow)+i];
			}

			if(cur==next && j && thisrun < 256) {
				thisrun++;
			} else {
				/* emit run */
				if(thisrun > 3 || cur==slug) {
					colbuf[colpos++]=slug;
					colbuf[colpos++]=cur;
					colbuf[colpos++]=(char)(thisrun-1);

				} else {
					for(k=0;k<thisrun;k++) {
						colbuf[colpos++]=(char)cur;
					}
				}
				thisrun=1;
			}
		} /* down col */

		/* write the column to magnetic media */
		emit2(colpos); /* size of column */
		emit1(slug);
		for(j=0;j<colpos;j++) {
			emit1(colbuf[j]);
		}
	} /* cur col */
	return 0;
}
