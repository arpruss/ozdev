
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

FILE *ofile;

int widths[256];
int offsets[256];
int redirects[256];
unsigned char font_data[8192];
unsigned char header[12];
int length;

#define lastChar (header[7])
#define bitmap_height (header[8])

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
        if((mask&0xFF)==0)
        {
            mask=1;
            p++;
        }
        if(mask&*p) fputc('X',f);
            else fputc('.',f);
    }


    fputc('\n',f);
}

main(int argc, char **argv)
{
    FILE *f;
    char name[1024];
    int length;
    int i,j;
    int rlen;

    if(argc<2) err("wzf2txt font\n"
                    "  You must have font.wzf on your system.\n");
    strcpy(name,argv[1]);
    strcat(name,".wzf");
    f=fopen(name,"rb");
    if(f==NULL) err("Cannot open %s.\n",argv[1]);
    if(fread(header,1,12,f)!=12 || header[2]!=0x6F || header[3]!=0x46)
        err("Error in header!");
    length=header[4]+256*(unsigned int)(header[5]);

    for(i=0;i<=lastChar;i++)
    {
        widths[i]=myfgetc(f);
        offsets[i]=getword(f)-12-(lastChar+1)*(int)3;
    }
    for(i=0;i<=lastChar;i++)
    {
        redirects[i]=i;
        if(32<=i && i<127) continue;
        for(j=32;j<127;j++)
        {
            if(offsets[i]==offsets[j])
            {
                redirects[i]=j;
                break;
            }
        }
    }
    rlen=length-12-(lastChar+1)*(int)3;
    if(rlen>8192) err("Font too big!\n");
    if((int)rlen!=(int)fread(font_data,1,rlen,f))
    {
        err("Cannot read font.\n");
    }

    strcpy(name,argv[1]);
    strcat(name,".txt");
    ofile=fopen(name,"w");
    if(ofile==NULL) err("Cannot open %s.\n",argv[1]);


    fprintf(ofile,
                  "%d (first)\n"
                  "%d (last)\n"
                  "%d (bitmap_height)\n"
                  "%d (line_height)\n",header[6],lastChar,
                    bitmap_height,header[9]);


    for(i=0;i<=lastChar;i++)
    {
        fprintf(ofile,"%d %d",i,redirects[i]);
        if(i!=redirects[i])
        {
            fprintf(ofile," (redirected)\n");
            continue;
        }
        else fprintf(ofile,"\n");
        for(j=0;j<bitmap_height;j++)
        {
            dumpline(ofile,widths[i],font_data+offsets[i]+(widths[i]>8?j*2:j));
        }
    }
    fclose(ofile);
    fclose(f);
    return 0;
}
