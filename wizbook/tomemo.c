#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#ifdef DLL
#include <setjmp.h>
static jmp_buf ExitJump;
#endif

#define EOF_MARKER 33
#define HEX 35
#define SPECIAL (HEX+16)
#define NUMSPECIALS (254-SPECIAL)
#define TOMEMO2_MARKER 254

long counts[256];
int xtable[256];

#define MAX_ARGC 5010

char *argv[MAX_ARGC];
int argc;

char *strippath(char *dir,char *s,int nodir,int stripext)
{
    static char output[1024];
    char *ext;
    char *p=s+strlen(s)-1;
        while(*p!='\\' && p>=s) p--;
        p++;
    if(stripext)
    {
        ext=strrchr(p,'.');
        if(ext!=NULL) *ext=0;
    }
    if(nodir)
        strcpy(output,p);
    else
    {
        strcpy(output,dir);
        strcat(output,"/");
        strcat(output,p);
    }
    return output;
}

static FILE *myfopen(char *name, char *mode)
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

void puthex(char *s,int x)
{
    s[0]=HEX+(x%16);
    s[1]=HEX+(x/16);
}

#define BUFFER_SIZE (1950)

int insert(int j,char *fname)
{
    char line[1024];
    char *p;
    FILE *f;
    int j0=j;

    f=myfopen(fname,"r");

    while(j<MAX_ARGC)
    {
        if(NULL==fgets(line,1024,f)) break;
        if(line[strlen(line)-1]=='\n') line[strlen(line)-1]=0;
        if(*line==0) continue;
        if(line[0]=='"')
        {
            p=strrchr(line,'"');
            *p=0;
            if(*line==0) continue;
            p=line+1;
        }
        else p=line;
        argv[j++]=strdup(p);
    }
    fclose(f);
    return j-j0;
}

#ifdef DLL
static
#endif
int main(int _argc, char **_argv)
{
    FILE *out;
    FILE *in;
    char name[256];
    char upload_to[1024];
    char outbuf[BUFFER_SIZE+1];
    time_t timer;
    unsigned short sum;
    unsigned short file_number;
    int buffer_pos;
    int i,j;
    int start_arg;
    int nodir=0;
    int stripext=0;
    int c,c2;
    unsigned id;

    argv[0]=_argv[0];
    for(j=i=1;i<_argc && j<MAX_ARGC;i++,j++)
    {
        if(_argv[i][0]=='@')
        {
            if(_argv[i][1]=='@') argv[j]=_argv[i]+2;
            else
            {
                j+=insert(j,_argv[i]+1)-1;
            }
        }
        else
            argv[i]=_argv[i];
    }
    argc=j;

    time(&timer);
    srand(timer);
    rand();
    file_number=(rand()<<12) | rand();
    if(argc<5)
    {
        fprintf(stderr,"tomemo outname owner_id directory file1 [...]\n");
        return 1;
    }
    for(start_arg=1;start_arg<argc-4 && argv[start_arg][0]=='-';start_arg++)
    {
        switch(argv[start_arg][1])
        {
            case 'n':
               nodir=1;
               break;
            case 'e':
               stripext=1;
               break;
            case 0:
               start_arg++;
               break;
        }
    }
    strcpy(name,argv[start_arg]);
    strcat(name,".wzd");
    out=myfopen(name,"wb");
    strcpy(upload_to,argv[start_arg+2]);
    strcat(upload_to,"/");
    strcat(upload_to,argv[start_arg+3]);
    if(argc-(start_arg+3)>1) strcat(upload_to,",...");
    fprintf(out,
"<SHARP WZD DATA>\r\n"
"<DATA TYPE>\r\n"
"MEMO\r\n"
"</DATA TYPE>\r\n"
"<TITLE>\r\n"
"Uploading: %s\r\n"
"</TITLE>\r\n"
"<CATEGORY>\r\n"
"NO CATEGORY\r\n"
"</CATEGORY>\r\n"
"<DESCRIPTION>\r\n"
"Upload to LIBOZ file system.  Run FromMemo (or some program with FromMemo capabilities) right after uploading to put data into C-based file system.\r\n"
"</DESCRIPTION>\r\n"
"<DATA>\r\n"
"Sharp Download Data V1.0\r\n"
"\"C\",\"M\",\" \"\r\n"
"\"I\",\"ATTR\",\"TTL1\",\"MEM1\",\"DATE\",\"IMG3\",\"CLAS\",\"CGR1\",\"QMEM\"\r\n"
"\"T\",\"Attribute\",\"Title\",\"text\",\"Create data\",\"bitmap\",\"File name\",\"Category kind\",\"Quick Memo\"\r\n",
upload_to);
    for(i=start_arg+3;i<argc;i++)
    {
        int partnumber=0;
        in=myfopen(argv[i],"rb");
        for(j=0;j<256;j++)
        {
            counts[j]=0;
            xtable[j]=-1;
        }
        while(-1!=(c=fgetc(in))) counts[c]++;
        rewind(in);
        for(j=0;j<NUMSPECIALS;j++)
        {
            long maxcount=-1;
            int maxk,k;
            for(k=0;k<256;k++)
            {
                if(counts[k]>maxcount)
                {
                    maxcount=counts[k];
                    maxk=k;
                }
            }
            xtable[maxk]=SPECIAL+j;
            counts[maxk]=-1;
        }
        buffer_pos=0;
        outbuf[buffer_pos++]=TOMEMO2_MARKER;
        for(j=0;j<NUMSPECIALS;j++)
        {
            int k;
            for(k=0;k<256;k++)
            {
                if(xtable[k]==SPECIAL+j)
                {
                    puthex(outbuf+buffer_pos,k);
                    buffer_pos+=2;
                    break;
                }
            }
        }
        do
        {
            sum=0;
            while(-1!=(c=fgetc(in)) && buffer_pos<BUFFER_SIZE-10)
            {
                sum+=(unsigned char)c;
                if(xtable[c]>=0) outbuf[buffer_pos++]=xtable[c];
                else
                {
                    puthex(outbuf+buffer_pos,c);
                    buffer_pos+=2;
                }
            }
            outbuf[buffer_pos++]=EOF_MARKER;
            outbuf[buffer_pos]=0;
            fprintf(out,"\"D\",\" \",\"~~z%05u,%05u,%05u\","
            "\"**LIBOZ_UPLOAD*%c%s\x1F"
            "%s%05hu\",\"20200101\",,,\"\",\r\n",atoi(argv[start_arg+1]),
              (unsigned short)(i+file_number),partnumber,c==-1?'L':'C',
                strippath(argv[start_arg+2],argv[i],nodir,stripext),outbuf,sum);
            partnumber++;
            buffer_pos=0;
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

