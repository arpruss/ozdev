#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

FILE *ofile;

int widths[256];
int offsets[256];
int redirects[256];
unsigned char font_data[8193];
int length;
int offset;
int maxwidth;
int firstChar;
int lastChar;
int bitmap_height;
int line_height;
int width;

int pos;

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

unsigned getword(FILE *f)
{
    unsigned x;
    x=myfgetc(f);
    return x+256*myfgetc(f);
}

void dumpline(FILE *f,int width, unsigned char *p)
{
    unsigned char mask;
    for(mask=1;width;mask<<=1,width--)
    {
        if(mask&0xFF==0)
        {
            mask=1;
            p++;
        }
        if(mask&*p) fputc('X',f);
            else fputc('.',f);
    }
    fputc('\n',f);
}

void getline(char *s,FILE *f)
{
    char *p;
    if(NULL==fgets(s,1024,f)) err("Error reading file!\n");
    p=strrchr(s,'\n');
    if(p!=NULL) *p=0;
}

void process_bin(unsigned short value,int width)
{
    if(offset>=8192) err("Font too big!");
    if(width<=8)
        font_data[offset++]=value;
    else
    {
        font_data[offset++]=value%256;
        font_data[offset++]=value/256;
    }
}

void process_line(char *s,int width)
{
    unsigned short value=0;
    while(*s)
    {
        value=(value>>1)|(((*s)!='.')<<(width-1));
        s++;
    }
    process_bin(value,width);
}

int xdigit(char c)
{
    char s[2];
    unsigned int x;
    s[0]=c;
    s[1]=0;
    sscanf(s,"%x",&x);
    return x;
}

char *scanuint(char *s,int *value)
{
    while(*s && isspace(*s)) s++;
    if(!*s) return NULL;
    if(*s=='&')
    {
        s++;
        *value=0;
        while(isxdigit(*s))
        {
            *value=(*value)*16+xdigit(*s);
            s++;
        }
        return s;
    }
    if(isdigit(*s))
    {
        *value=0;
        while(isdigit(*s))
        {
            *value=(*value)*10+(*s-'0');
            s++;
        }
        return s;
    }
    if(*s=='\'') s++;
    *value=(unsigned char)(*s++);
    return s;
}

main(int argc, char **argv)
{
    FILE *f;
    char name[1024];
    char line[1024];
    char *p;

    int i,j,k;
    unsigned short csum;
    int rlen;
    int rto;

    if(argc<2) err("txt2wzf font\n"
                    "  You must have font.txt on your system.\n");
    strcpy(name,argv[1]);
    strcat(name,".txt");
    f=fopen(name,"r");
    if(f==NULL) err("Cannot open %s.\n",argv[1]);
    getline(line,f);
    length=0;
    sscanf(line,"%d",&firstChar);
    if(firstChar!=0) err("Unsupported firstChar");
    getline(line,f);
    sscanf(line,"%d",&lastChar);
    if(lastChar!=255 && lastChar!=127) err("Unsupported lastChar\n");
    getline(line,f);
    sscanf(line,"%d",&bitmap_height);
    getline(line,f);
    sscanf(line,"%d",&line_height);

    font_data[2]=0x6F;
    font_data[3]=0x46;
    font_data[6]=firstChar;
    font_data[7]=lastChar;
    font_data[8]=bitmap_height;
    font_data[9]=line_height;
    /* max width needs to be calculated below */
    font_data[11]=0;

    maxwidth=0;
    offset=12+3*(lastChar+1);

    for(i=0;i<=lastChar;i++) redirects[i]=-1;

    for(;;)
    {
        if(NULL==fgets(line,1023,f)) break;
        if(argc>2) printf("%s",line);
        p=scanuint(line,&j);
        if(p==NULL) break;
        p=scanuint(p,redirects+j);
        if(p==NULL) redirects[j]=j;
        if(j!=redirects[j]) continue;
        getline(line,f);
        if(argc>2) puts(line);
        width=strlen(line);
        widths[j]=width;
        if(width<2) width=2;
        if(width>maxwidth) maxwidth=width;
        offsets[j]=offset;
        process_line(line,width);
        for(k=1;k<bitmap_height;k++)
        {
            getline(line,f);
            if(argc>2) puts(line);
            process_line(line,width);
        }
    }

    if(redirects['*']==-1)
    {
        for(rto=0;redirects[rto]==-1 && rto<=lastChar;rto++);
        if(rto>lastChar)
        {
            err("No characters found!");
        }
    }
    else rto='*';
    for(i=0;i<=lastChar;i++) if(redirects[i]==-1) redirects[i]=rto;

    for(i=0;i<=lastChar;i++)
    {
        font_data[12+i*3]=widths[redirects[i]];
        font_data[12+i*3+1]=offsets[redirects[i]]%256;
        font_data[12+i*3+2]=offsets[redirects[i]]/256;
    }

    fclose(f);

    strcpy(name,argv[1]);
    strcat(name,".wzf");
    ofile=fopen(name,"wb");
    if(ofile==NULL) err("Cannot open %s.\n",argv[1]);
    length=offset;
    font_data[4]=length%256;
    font_data[5]=length/256;
    font_data[10]=maxwidth;

    for(csum=0,i=2;i<length;i++)
    {
        csum+=font_data[i];
    }

    font_data[0]=csum%256;
    font_data[1]=csum/256;

    fwrite(font_data,1,length,ofile);
    fclose(ofile);

    return 0;
}
