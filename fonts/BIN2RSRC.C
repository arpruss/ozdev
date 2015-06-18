/* extract the resource fork from a MacBinary file */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
        ftell(f)!=x) err("Error seeking!");
    return;
}

unsigned short readword(FILE *f)
{
  unsigned short x;
  x=myfgetc(f);
  return x*256+myfgetc(f);
}


unsigned long readdword(FILE *f)
{
    unsigned long x;
    x=65536ul*readword(f);
    return x+readword(f);
}


main(int argc, char **argv)
{
    char name[1024];
    FILE *in,*out;
    unsigned long dforklen,rforklen;
    if(argc!=2) err("bin2rsrc fname\n");
    strcpy(name,argv[1]);
    strcat(name,".bin");
    in=myfopen(name,"rb");
    myseek(in,83);
    dforklen=readdword(in);
    rforklen=readdword(in);
    printf("Data fork length = %lu\n"
           "Resource fork length = %lu\n",
            dforklen,rforklen);
    myseek(in,128+dforklen);
    strcpy(name,argv[1]);
    strcat(name,".rsrc");
    out=myfopen(name,"wb");
    while(rforklen)
    {
        fputc(myfgetc(in),out);
        rforklen--;
    }
    fclose(in);
    fclose(out);
    return 0;
}
