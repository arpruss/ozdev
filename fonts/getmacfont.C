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

void process(FILE *f,long namepos,unsigned long datapos)
{
    byte name[1040];
    byte namelen;
    static int count=0;
    FILE *out;
    int i;
    unsigned long len;
    if(namepos==-1)
        sprintf(name,"%s%d.data",thename,count++);
    else
    {
        myseek(f,namepos);
        namelen=getbyte(f);
        if((int)fread(name,1,namelen,f)!=(int)namelen) err("Error reading resource name\n");
        name[namelen]=0;
        for(i=0;i<namelen;i++)
        {
            if(name[i]>126 || name[i]<32 ||
                name[i]=='\\' || name[i]=='?' || name[i]==':'
                || name[i]=='/' || name[i]=='%' || name[i]=='<'
                || name[i]=='>' || name[i]=='*' || name[i]=='"')
                name[i]='_';
        }
        strcat(name,".data");
    }
    out=myfopen(name,"wb");
    printf("Extracting: %s\n",name);
    myseek(f,datapos);
    len=getdword(f);
    while(len--)
        fputc(myfgetc(f),out);
    fclose(out);
}

main(int argc, char **argv)
{
    int i;
    char name[1024];
    char rsrc[5]="NFNT";
    char s[5];
    char *p;
    unsigned short n;
    int rsrconly=0;
    FILE *in,*out;
    unsigned long dforklen,rforklen,pos;
    if(argc!=2 && argc!=3)
        err("getmacfont filename [RSRC]\n"
            "  This extracts all the bitmapped font (NFNT) resources\n"
            "  from the specified MacBinary encoded file.\n"
            "  You can also use it to extract any other type of resource\n"
            "  by supplying the resource type on the commandline,\n"
            "  for instance: getmacfont input.bin FOND.\n"
            "  Each resource found gets saved into a separate file with\n"
            "  filename ending in .data.\n");
    strcpy(name,argv[1]);
    if(strlen(argv[1])>5 && !strcmp(argv[1]+strlen(argv[1])-5,".rsrc")) rsrconly=1;
    if(argc==3)
    {
        if(strlen(argv[2])!=4) err("Resource name should be four characters.\n");
        strcpy(rsrc,argv[2]);
    }
    in=myfopen(name,"rb");
    strcpy(thename,name);
    if((p=strrchr(thename,'.'))!=NULL) *p=0;
    if(!rsrconly)
    {
        myseek(in,83);
        dforklen=getdword(in);
        rforklen=getdword(in);
        printf("Data fork length = %lu\n"
           "Resource fork length = %lu\n",
            dforklen,rforklen);
        myseek(in,rforkpos=128+dforklen);
    }
    else
    {
        rforkpos=0;
    }
    rdatapos=rforkpos+getdword(in);
    rmappos=rforkpos+getdword(in);
    rdatalen=getdword(in);
    rmaplen=getdword(in);
    myseek(in,rmappos+16+4+2+2);
 /*   printf("%lx\n",rmappos); */
    rtypepos=rmappos+getword(in);
 /*   printf("%lx\n",rtypepos); */
    rnamepos=rmappos+getword(in);
 /*   printf("%lx\n",rnamepos); */
    myseek(in,rtypepos);
    numtypes=getword(in)+1;
    for(i=0;i<numtypes;i++)
    {
        read4bytes(in,s);
        if(!strcmp(rsrc,s)) break;
        getword(in);
        getword(in);
    }
    if(i==numtypes) err("Cannot find resources of type %s.\n",rsrc);
    myresnum=1+getword(in);
    myreflistpos=rtypepos+getword(in);
    myseek(in,myreflistpos);
    for(i=0;i<(int)myresnum;i++)
    {
        getword(in); /* Resource ID */
        mynamepos=rnamepos+(n=getword(in));
        getbyte(in); /* Resource attributes */
        mydatapos=rdatapos+get3bytes(in);
        getdword(in);
        pos=ftell(in);
        process(in,n==65535?-1:mynamepos,mydatapos);
        myseek(in,pos);
    }
    fclose(in);
    return 0;
}
