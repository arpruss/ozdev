#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

FILE *ofile;
int as=0;

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



void writebyte(unsigned char c)
{
    if(pos!=0)
    {
        if(as)
        {
            if((pos%16)) fputc(',',ofile);
        }
        else
        {
            fputc(',',ofile);
        }
    }
    if(!(pos%16))
    {
        if(as) fprintf(ofile,"\ndefb ");
            else fprintf(ofile,"\n ");
    }
    fprintf(ofile,"%u",(unsigned int)c);
    pos++;
}

main(int argc, char **argv)
{
    FILE *f;
    unsigned char header[12];
    char name[1024];
    int length;
    int i,j;

    if(argc<2) err("wzf2prog [-a] font\n"
                    "  You must have font.wzf on your system.\n"
                    "  Use -a to generate assembly format.\n");
        for(j=1;j<argc-1;j++)
        {
            switch(argv[j][1])
            {
                case 'a':
                    as=1;
                    break;
                default:
                    err("wzf2prog: unknown option\n");
            }
        }
    strcpy(name,argv[j]);
    strcat(name,".wzf");
    f=fopen(name,"rb");
    if(f==NULL) err("Cannot open %s.\n",argv[j]);
    if(fread(header,1,12,f)!=12 || header[2]!=0x6F || header[3]!=0x46)
        err("Error in header!");
    length=header[4]+256*(unsigned int)(header[5]);
    strcpy(name,argv[j]);
    strcat(name,as?".as":".c");
    ofile=fopen(name,"w");
    if(ofile==NULL) err("Cannot open %s.\n",name);
    if(isdigit(argv[j][0]))
    {
        strcpy(name+1,argv[j]);
        name[0]='_';
    }
    else
        strcpy(name,argv[j]);
    if(as)
    {
        fprintf(ofile,"%s_length equ %d\n"
                      "_%s_length equ %d\n"
                      "global _%s_data\n"
                      "%s_data:\n"
                      "_%s_data:\n",name,length,name,length,name,name,name);
    }
    else
    {
        fprintf(ofile,"#define %s_length %d\n"
                      "unsigned char %s_data[]=\n"
                      "{\n",name,length,name);
    }
    pos=0;
    for(i=0;i<12;i++) writebyte(header[i]);
    for(i=12;i<length;i++)
        writebyte(myfgetc(f));
    if(as) fprintf(ofile,"\n");
    else
    fprintf(ofile,"\n};\n"
                  "#ifdef LOAD_FONT\n"
                  "#include <oz.h>\n"
                  "main(){static unsigned c;static byte slot;\n"
                  "ozputs(0,10,\"Put %s into slot 0/1 (or ESC to cancel)?\");\n"
                  "c=ozgetch();if(c=='0' || c=='1') slot=c-'0'; else return;\n"
                  "ozwritecustomfontmem(slot,%s_data);ozdelay64hz(16);}\n"
                  "#endif\n",name,
                  name,name);
    fclose(ofile);
    fclose(f);
    return 0;
}
