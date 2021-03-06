#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_FONT_LEN 3456
#define MAX_FONT_NAME 20
#include "xlat.c"
#include "tomac.c"
#include "ascii2me.c"
#include "xgreek.c"

unsigned char the_font[MAX_FONT_LEN*2];
#define MAX_FONT_LEN 3456
#define FONT_HEADER 12
#define MIN_WIDTH_TABLE (128*3)
#define FONT_DATA (MAX_FONT_LEN*2-FONT_HEADER-MIN_WIDTH_TABLE)
unsigned char font_data[MAX_FONT_LEN*2];
unsigned char font_bits[8192];

unsigned short header[13];
unsigned short inoffsets[257];
int font_widths[256];
unsigned char font_left_offsets[256];
unsigned offsets[256];
int disablekerning=0;
int raw=0;
int start;
int offset;
int widths[256];
int maxwidth;
int xmac=0;
int rev=0;
int dump=0;
int greek=0;
int ystart;
int yend;
int squishdown=0;
int upto=127;
int upto0;
int macfont=0;
int memoedit_force=0;
int asciionly=0;
int maxkerning;
int left_delta=0;
int right_delta=0;

#define firstChar header[1]
#define lastChar header[2]
#define maxWidth header[3]
#define fRectWidth header[6]
#define fRectHeight header[7]
#define rowWords header[12]
#define owTLoc header[8]
#define kernMax header[4]

int xlat(int c)
{
    if(raw) return c;
    if(greek && c>=128 && c<128+49)
        return xgreek[c-128];
    if(c==127)
        return macfont?mac2memoedit[0]:palm2memoedit[0];
    if(c==13 || c>=32 && (c<128 || !macfont)) return c;
    if(c>=128 && xmac) return extcharset_mac[c-128];
    if(macfont) return mac2memoedit[c];
        else return palm2memoedit[c];
}

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

int myfgetc(FILE *f)
{
    int c=fgetc(f);
    if(c==-1) err("Error reading font!");
    return c;
}

unsigned short readword(FILE *f)
{
  unsigned short x;
  x=myfgetc(f);
  return x*256+myfgetc(f);
}

int getpixel(int x, int y)
{
   int col,mask;
   col=x/8;
   mask=1<<(7-(x%8));
   return !!(mask&font_bits[y*rowWords*2+col]);
}

void putpixel(int x, int y, int c)
{
   int col,mask;
   col=x/8;
   mask=1<<(7-(x%8));
   if(c) font_bits[y*rowWords*2+col]|=mask;
    else font_bits[y*rowWords*2+col]&=~mask;
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
    int botline;
    int topline;
    offsets[13]=offset;
    i=0;
    if(h<3) return;
    if(h<6 || fRectWidth<6)
    {
        puts("Generating small-style CR");
        setwidth(13,5);
        for(;i<h-3;i++)
            addbyte(0x08);
        addbyte(0x0a); /* h-3 */
        addbyte(0x0f); /* h-2 */
        addbyte(0x02); /* h-1 */
    }
    else
    {
        puts("Generating large-style CR");
        setwidth(13,6);
        if(h>6) botline=1;
        for(;i<h-botline-5;i++)
        {
            if(i<h-11 || h>7 && i==0) addbyte(0);
             else addbyte(0x10);
        }
        addbyte(0x14);
        addbyte(0x12);
        addbyte(0x1f);
        addbyte(0x02);
        addbyte(0x04);
        if(botline) addbyte(0);
    }
}


int getstart(int c)
{
    int x,y;
    int x0,x1;
    x0=inoffsets[c-firstChar];
    x1=inoffsets[c+1-firstChar];
    for(y=0;y<fRectHeight;y++)
     for(x=x0;x<x1;x++) if(getpixel(x,y)) return y;
    return fRectHeight;
}

int getend(int c)
{
    int x,y;
    int x0,x1;
    x0=inoffsets[c-firstChar];
    x1=inoffsets[c+1-firstChar];
    for(y=fRectHeight-1;y>=0;y--)
     for(x=x0;x<x1;x++) if(getpixel(x,y)) return y;
    return 0;
}

int realwidth(int c)
{
    int x,y;
    int x0,x1;
    x0=inoffsets[c-firstChar];
    x1=inoffsets[c+1-firstChar];
    for(x=x1-1;x>=x0;x--)
     for(y=0;y<fRectHeight;y++)
        if(getpixel(x,y)) return x-x0+1;
    return 0;
}

int havepixel(int x0,int y,int width)
{
    int x;
    for(x=x0;x<x0+width;x++) if(getpixel(x,y)) return 1;
    return 0;
}

void shiftdownrows(int x0,int width,int num)
{
    int x,y;
    if(!num) return;
    for(y=yend;y>=ystart+num;y--) for(x=x0;x<x0+width;x++)
      putpixel(x,y,getpixel(x,y-num));
}

void grab_char(int c)
{
  int xc;
  int x,y;
  int x1,x0;
  int width;
  int rw;
  int origwidth;
  int delta;
  unsigned short value;
  offsets[c]=offset;
  xc=xlat(c);
  x0=inoffsets[xc-firstChar];
  x1=inoffsets[xc+1-firstChar];
  delta=left_delta+font_left_offsets[xc-firstChar];
  if(delta<0) delta=0;
  width=delta+font_widths[xc-firstChar];
  if(width+right_delta<16) width+=right_delta-font_left_offsets[xc-firstChar];
  if(width<(rw=realwidth(xc)))
  {
     if(dump) printf("%u kerned out of box--adjusting\n",c);
     width=rw;
     delta=left_delta;
  }
  if(dump)
    printf("%c=%u=0x%X [%u=0x%X]   width=%d, left=%d\n",c<32?' ':c,(unsigned)c,(unsigned)c,(unsigned)xc,
        (unsigned)xc,width,delta);
  if(width>16)
  {
    printf("Character #%d too wide--trimming.\n",c);
    width=16;
  }
  setwidth(c,(width<2)?2:width);
  if(squishdown)
  {
     for(y=ystart;y<ystart+squishdown;y++)
        if(havepixel(x0,y,x1-x0)) break;
     shiftdownrows(x0,x1-x0,squishdown-(y-ystart));
  }
  for(y=ystart+squishdown;y<=yend;y++)
  {
     value=0;
     for(x=x0;x<x1;x++)
     {
       value=(value>>1)|(getpixel(x,y)<<(x1-x0-1+delta));
       if(dump)
       {
        if(getpixel(x,y)) putchar('X'); else putchar('.');
       }
     }
     if(width>8)
     {
      addbyte(value%256);
      addbyte(value/256);
     }
     else addbyte(value);
     if(dump) putchar('\n');
  }
}

  

FILE *binfile;

int pos;
unsigned short checksum;

void writebyte(unsigned char c)
{
    fputc(c,binfile);
    pos++;
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

FILE *myfopen(char *s,char *m)
{
    FILE *f=fopen(s,m);
    if(f==NULL) err("Cannot open %s.\n",s);
    return f;
}

void makeident(char *out,char *in)
{
    register char *p=in;
    while(*p)
    {
        if(isalpha(*p)) *out++=*p++;
         else if(*p=='_') *out++=*p++;
          else if(isdigit(*p))
          {
            if(p==in) *out++='_';
            *out++=*p++;
          }
          else
          {
            *out++='_';
            p++;
          }
    }
    *out=0;
    return;
}

void cleanname(char *out,char *in,char *def)
{
    register char *p=out;
    int len;
    len=strlen(in);
    if(len>5 && !strcmp(in+len-4,"-1.5"))
    {
        in[len-4]=0;
    }
    else if(len>4 && !strcmp(in+len-3,"1.5"))
    {
        in[len-3]=0;
    }
    while(*in)
    {
        if(isdigit(*in) || isalpha(*in))
        {
            if(isdigit(*in) && p==out)
            {
                *p++='_';
            }
            *p++=*in++;
        }
        else if(*in=='.' || *in=='-')
        {
            *p++='_';
            in++;
        }
        else in++;
    }
    *p=0;
    if(p-out<3) strcpy(out,def);
    out[MAX_FONT_NAME]=0;
}

main(int argc, char **argv)
{
  char name[1024];
  char markers[6]="";
  char fontname[MAX_FONT_NAME+1];
  FILE *f;
  unsigned short n;
  int i;
  int v;
  int c;
  int length;
  int height;
  int highest_start,lowest_end;
  int resourceonly=0;
  if(argc<2) err("palm2wiz font [d] [n] [a] [e] [E] [r] [q] [M] [m] [L] [R]\n"
                "  You must have font.pdb or font.data on your system in the right format.\n");
if(argc>2)
{
    for(i=2;i<argc;i++)
    {
        switch(argv[i][0])
        {
            case 'n':
                disablekerning=1;
                break;
            case 'a':
                asciionly=1;
                upto=127;
                memoedit_force=0;
                break;
            case 'e':
                upto=255;
                asciionly=0;
                memoedit_force=1;
                break;
            case 'E':
                memoedit_force=1;
                upto=255;
                macfont=1;
                xmac=1;
                asciionly=0;
                break;
            case 'r':
                resourceonly=1;
                break;
            case 'd':
                dump=1;
                break;
            case 'q':
                squishdown=atoi(argv[i]+1);
                break;
            case 'M':
                memoedit_force=1;
                asciionly=0;
                break;
            case 'l':
            case 'L':
                left_delta=atoi(argv[i]+1);
                break;
            case 'R':
                right_delta=atoi(argv[i]+1);
                break;
            case 'm':
                macfont=1;
                break;
            case 'G':
                greek=1;
                upto=255;
                break;
            case 'W':
                raw=1;
                break;
            default:
                err("font2wiz: unknown option\n");
        }
    }
}
    if(asciionly) upto0=126;
        else upto0=upto;
    strcpy(name,argv[1]);
    strcat(name,resourceonly?".data":".pdb");
    f=myfopen(name,"rb");
    if(resourceonly)
    {
        strcpy(name,argv[1]);
    }
    else
    {
        fread(name,1,MAX_FONT_NAME,f);
    }
    name[MAX_FONT_NAME]=0;
    for(i=0;i<256;i++) widths[i]=-1;
    maxwidth=offset=0;
    if(resourceonly)
    {
        start=16;
        header[0]=readword(f);
    }
    else
    for(i=MAX_FONT_NAME;i<200;i++)
    {
        start=16+ftell(f);
        if((header[0]=readword(f))==0x9000) break;
    }
    if(i==200) err("Not NFNT font!");
    for(i=1;i<13;i++) header[i]=readword(f);
    if((int)(rowWords*2*fRectHeight)>8192
    || (int)fread(font_bits,1,rowWords*2*fRectHeight,f)<(signed int)(rowWords*2*fRectHeight))
        err("Error in font");
    for(i=0;i<lastChar-firstChar+2;i++)
    {
      if(i<257) inoffsets[i]=readword(f);
        else readword(f);
    }
    if(dump) printf("start:%d pos:%d owtable:%d\n",start,ftell(f),start+owTLoc*2);
    fseek(f,start+owTLoc*2,SEEK_SET);
    if(start+owTLoc*2 !=ftell(f)) err("Error seeking!\n");
    if(kernMax<=32767u) maxkerning=-(long)kernMax;
        else maxkerning=65536l-(long)kernMax;
    for(i=0;i<lastChar-firstChar+1 && i<=256;i++)
    {
        int v;
        int k,w;
        k=myfgetc(f);
        w=myfgetc(f);
        if(dump)
            printf("Width data for %d: %d %d\n",i+firstChar,k,w);
        if(k==255 && w==255)
        {
            font_widths[i]=maxWidth;
            font_left_offsets[i]=0;
            continue;
        }
        v=k-maxkerning;
        if(v<0 || disablekerning) font_left_offsets[i]=0;
            else font_left_offsets[i]=v;
        font_widths[i]=w;
        if(font_widths[i]>16) font_widths[i]=16;
/*        printf("%c %d %d\n",i+firstChar,font_left_offsets[i],
            font_widths[i]); */
    }
    fclose(f);

    printf("Input font set: %d-%d\n",firstChar,lastChar);

    if(upto==255 && lastChar<255)
    {
        puts("Do not have that many characters, trimming set.");
    }

    cleanname(fontname,name,argv[1]);

    if(squishdown)
        strcat(markers,"Q");
    if(asciionly)
        strcat(markers,"A");
    if(macfont && upto==255 && !xmac)
        strcat(markers,"M");
    if(upto==255 && (xmac || !macfont))
        strcat(markers,"E");

    if(*markers)
    {
        int lenname;
        int lenmarkers=strlen(markers);
        strcat(fontname,"_");
        lenname=strlen(fontname);
        if(lenname+lenmarkers>MAX_FONT_NAME)
            strcpy(fontname+MAX_FONT_NAME-lenmarkers,markers);
        else
            strcat(fontname,markers);
    }

    strcpy(name,fontname);
    strcat(name,".wzf");
    binfile=myfopen(name,"wb");

    highest_start=fRectHeight;
    lowest_end=0;
    for(i=(memoedit_force || raw)?1:32;i<=upto0;i++)
    {
        if(i==13 || i>lastChar) continue;
        v=getstart(xlat(i));
        if(dump) printf("%d : start y=%d, ",i,v);
        if(v<highest_start)
        {
            highest_start=v;
        }
        v=getend(xlat(i));
        if(dump) printf("end y=%d\n",v);
        if(v>lowest_end) lowest_end=v;
    }
    if(highest_start>=lowest_end)
    {
        yend=ystart=lowest_end;
    }
    else
    {
        yend=lowest_end;
        ystart=highest_start;
    }
    height=yend-ystart+1-squishdown;
    printf("Recommended height=%d\n"
           "Actual height=%d\n",
             fRectHeight,
             height);
    if(asciionly) addcr(height);
    if(dump) printf("Scanning up to %d\n",upto0);
    for(i=asciionly?32:1;i<=upto0 && i<=lastChar;i++)
    {
        if(i==13)
            addcr(height);
        else
        {
            if(!memoedit_force && (i<32 || i==127) && (getstart(xlat(i))<ystart ||
                   getend(xlat(i))>yend))
            {
                if(dump) printf("Skipping %d [%d]\n",i,xlat(i));
                continue;
            }
            if(font_widths[xlat(i)-firstChar]!=255) grab_char(i);
        }
    }
    checksum=pos=0;
    dumpword(0); /* checksum */
    dumpword(0x466F); /* magic */
    dumpword(length=offset+(upto+1)*3+FONT_HEADER); /* length */
    printf("length=%d\n",length);
    dumpbyte(0);
    dumpbyte(upto);
    dumpbyte(height); /* actual line height */
    dumpbyte(fRectHeight); /* recommended line height */
    dumpbyte(maxwidth);
    dumpbyte(0); /* active */
    if(greek)
    {
        for(i=128+49;i<256;i++)
            widths[i]=-1;
    }
    for(i=0;i<=upto;i++)
    {
        if(widths[i]<0)
        {
            if(i<32) c=ascii2me[i];
                else if(i==127) c='e';
                    else c='*';
        }
        else c=i;
        dumpbyte(widths[c]);
 /*       printf("%d ",widths[c]); */
        dumpword(offsets[c]+(upto+1)*3+FONT_HEADER);
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
