#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#ifdef DLL
#include <setjmp.h>
static jmp_buf ExitJump;
#endif

char *strippath(char *s)
{
    char *p=s+strlen(s)-1;
    while(*p!='\\' && p>=s) p--;
    p++;
    return p;
}


FILE *myfopen(char *name, char *mode)
{
    FILE *f=fopen(name,mode);
    if(f==NULL)
    {
#ifdef DLL
        longjmp(ExitJump,2);
#else
        fprintf(stderr,"Cannot open `%s'\n",name);
        exit(3);
#endif
    }
    return f;
}

#define BUFFER_SIZE (1950)

#ifdef DLL
static
#endif
int main(int argc, char **argv)
{
    FILE *out;
    FILE *in;
    char name[256];
    char outbuf[BUFFER_SIZE+1];
    time_t timer;
    unsigned short sum;
    unsigned short file_number;
    int buffer_pos;
    int asciimode=0;
    int i;
    int c,c2;
    unsigned id;
    time(&timer);
    srand(timer);
    rand();
    file_number=(rand()<<12) | rand();
    if(argc<5)
    {
        fprintf(stderr,"tomemo outname owner_id directory file1 [...]\n");
        return 1;
    }
    strcpy(name,argv[1]);
    strcat(name,".wzd");
    out=myfopen(name,"wb");
    fprintf(out,
"<SHARP WZD DATA>\r\n"
"<DATA TYPE>\r\n"
"MEMO\r\n"
"</DATA TYPE>\r\n"
"<TITLE>\r\n"
"Upload to %s/\r\n"
"</TITLE>\r\n"
"<CATEGORY>\r\n"
"NO CATEGORY\r\n"
"</CATEGORY>\r\n"
"<DESCRIPTION>\r\n"
"Upload to LIBOZ file system\r\n"
"</DESCRIPTION>\r\n"
"<DATA>\r\n"
"Sharp Download Data V1.0\r\n"
"\"C\",\"M\",\" \"\r\n"
"\"I\",\"ATTR\",\"TTL1\",\"MEM1\",\"DATE\",\"IMG3\",\"CLAS\",\"CGR1\",\"QMEM\"\r\n"
"\"T\",\"Attribute\",\"Title\",\"text\",\"Create data\",\"bitmap\",\"File name\",\"Category kind\",\"Quick Memo\"\r\n",
argv[3]);
    for(i=4;i<argc;i++)
    {
        int partnumber=0;
        in=myfopen(argv[i],"rb");
        do
        {
            buffer_pos=0;
            sum=0;
            while(-1!=(c=fgetc(in)) && buffer_pos<BUFFER_SIZE-10)
            {
                sum+=(unsigned char)c;
                if(32<=c && c<'~' && c!='"' && c!='\\')
                {
                    if(asciimode)
                        outbuf[buffer_pos++]=c;
                    else
                    {
                        if(32<=(c2=fgetc(in)) && c2<'~' && c2!='"'
                          && c2!='\\')
                        {
                            ungetc(c2,in);
                            outbuf[buffer_pos++]='~';
                            outbuf[buffer_pos++]=c;
                            asciimode=1;
                        }
                        else
                        {
                            if(c2!=-1) ungetc(c2,in);
                            sprintf(outbuf+buffer_pos,"%02x",c);
                            buffer_pos+=2;
                            asciimode=0;
                        }
                    }
                }
                else
                {
                    if(asciimode) outbuf[buffer_pos++]='~';
                    asciimode=0;
                    sprintf(outbuf+buffer_pos,"%02x",c);
                    buffer_pos+=2;
                }
            }
            if(asciimode) outbuf[buffer_pos++]='~';
            outbuf[buffer_pos++]='*';
            outbuf[buffer_pos++]='*';
            outbuf[buffer_pos]=0;
            asciimode=0;
            fprintf(out,"\"D\",\" \",\"~~z%05u,%05u,%05u\","
            "\"**LIBOZ_UPLOAD*%c%s/%s\x1F"
            "%s%05hu\",\"20200101\",,,\"\",\r\n",atoi(argv[2]),
              (unsigned short)(i+file_number),partnumber,c==-1?'L':'C',argv[3],strippath(argv[i]),outbuf,sum);
            partnumber++;
            if(c!=-1) ungetc(c,in);
        } while(c!=-1);
        fclose(in);
    }
    fprintf(out,"</DATA>\r\n");
    fclose(out);
    return 0;
}

#ifdef DLL
int __stdcall tomemo(char *arg1, char *arg2, char *arg3, char *arg4)
{
    char *argv[5];
    argv[0]="tomemo";
    argv[1]=arg1;
    argv[2]=arg2;
    argv[3]=arg3;
    argv[4]=arg4;
    if(setjmp(ExitJump))
        return 1;
    else
        return main(5,argv);
}
#endif

