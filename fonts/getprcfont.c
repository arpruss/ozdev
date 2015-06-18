#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef unsigned char byte;

unsigned long rforkpos;
unsigned long rdatapos;
unsigned long rmappos;
unsigned long rdatalet;
unsigned long rmaplen;
unsigned long rtypepos;
unsigned long rnamepos;
byte numtypes;
unsigned int myresnum;
unsigned long myreflistpos;
unsigned long mynamepos;
unsigned long mydatapos;
unsigned long rdatalen;
char thename[1040];

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
    fcloseall();
    exit(1);
#endif
}

FILE *myfopen(char *s,char *m)
{
    FILE *f=fopen(s,m);
    if(f==NULL) err("Cannot open %s.\n",s);
    return f;
}

int myfgetc(FILE *f)
{
    int c=fgetc(f);
    if(c==-1) err("Error reading font!");
    return c;
}

void myseek(FILE *f, unsigned long x)
{
    if(fseek(f,x,SEEK_SET)<0 ||
        ftell(f)!=(long)x) err("Error seeking!");
    return;
}

unsigned char getbyte(FILE *f)
{
    int c;
    if(-1==(c=fgetc(f))) err("Error reading file.\n");
    return c;
}

unsigned short getword(FILE *f)
{
    unsigned short x;
    x=getbyte(f);
    return x*256+getbyte(f);
}

unsigned long getdword(FILE *f)
{
    unsigned short x;
    x=getword(f);
    return x*65536l+getword(f);
}

unsigned long get3bytes(FILE *f)
{
    unsigned short x;
    x=getbyte(f);
    return x*65536l+getword(f);
}

void read4bytes(FILE *f,char *s)
{
    int i;
    for(i=0;i<4;i++) s[i]=getbyte(f);
    s[4]=0;
}

void process(FILE *f,long len,unsigned long datapos)
{
    byte name[1040];
    byte namelen;
    static int count=0;
    FILE *out;
    int i;
    sprintf(name,"%s%d.data",thename,count++);
    out=myfopen(name,"wb");
    printf("Extracting: %s\n",name);
    myseek(f,datapos);
    while(len-- && !feof(f))
        fputc(myfgetc(f),out);
    fclose(out);
}

main(int argc, char **argv)
{
    int i;
    char name[1024];
    char rsrc[5]="FONT";
    char s[5];
    char *p;
    unsigned short n;
    FILE *in,*out;
    unsigned long pos,pos2,pos3;
    if(argc!=2 && argc!=3)
        err("getprcfont filename [RSRC]\n"
            "  This extracts all the bitmapped font (FONT) resources\n"
            "  from the specified PalmPilot .PRC file.\n"
            "  You can also use it to extract any other type of resource\n"
            "  by supplying the resource type on the commandline,\n"
            "  for instance: getprcfont input.prc ATTR.\n"
            "  Each resource found gets saved into a separate file with\n"
            "  filename ending in .data.\n");
    strcpy(name,argv[1]);
    if(argc==3)
    {
        if(strlen(argv[2])!=4) err("Resource name should be four characters.\n");
        strcpy(rsrc,argv[2]);
    }
    in=myfopen(name,"rb");
    strcpy(thename,name);
    if((p=strrchr(thename,'.'))!=NULL) *p=0;
    myseek(in,0x4C);
    numtypes=getword(in);
    for(i=0;i<numtypes;i++)
    {
        read4bytes(in,s);
        getword(in);
        pos2=getdword(in);
        pos=ftell(in);
        if(i<numtypes-1)
        {
            getdword(in);
            getword(in);
            pos3=getdword(in);
        }
        else
        {
            pos3=0x7FFFFFFF;
        }
        if(!strcmp(rsrc,s))
        {
            process(in,pos3-pos2,pos2);
        }
        myseek(in,pos);
    }
    fclose(in);
    return 0;
}
