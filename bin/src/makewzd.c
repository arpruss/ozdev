#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifndef __unix__
#include <io.h>
#endif
#include <fcntl.h>

#define MAX_DESC 65536
#define MAX_TITLE 40
char title[MAX_TITLE+1];
char description[MAX_DESC+1];

char *randname(void);
unsigned long getlength(char *s);

/* BASIC ICON */
/*static char icon[]={
0x18, 0xFC, 0x0F, 0x02, 0x0C, 0x01, 0x0A, 0xFF, 0x09, 0x01, 0x0D, 0x3D,
0x0D,
0x45, 0x0D, 0x3D, 0x0D, 0x45, 0x0D, 0x3D, 0x0D, 0x01, 0x0D, 0xFF, 0x07
};
*/
/* C Program Icon */
/*static char icon[]={
0x18, 0x00, 0x00, 0x78, 0x00, 0x84, 0x00, 0x02, 0x00, 0xe2, 0x03, 0x22,
0x04,
0x22, 0x04, 0xe2, 0x03, 0x22, 0x00, 0xa4, 0x00, 0x78, 0x00, 0x20, 0x00 };
*/
/* New C Program Icon */
static char icon[]={
0x18, 0xFF, 0x03, 0x01, 0x06, 0xF1, 0x0E, 0xF9, 0x09, 0x99, 0x09, 0x19, 0x08, 0x19,
0x08, 0x99, 0x09, 0xF9, 0x09, 0xF1, 0x08, 0x01, 0x08, 0xFF, 0x0F };

#ifndef __unix__
unsigned long getlength(char *s)
{
   int h;
   unsigned long l;
   if(!(h=open(s,O_BINARY|O_RDONLY))) return 0;
   l=filelength(h);
   close(h);
   return l;
}
#else
unsigned long getlength(char *s)
{
    unsigned long l;
    FILE *fh=fopen(s,"rb");
    if(!fh) return 0;
    fseek(fh,0,SEEK_END);
    l=ftell(fh);
    fclose(fh);
    return l;
}
#endif

char *randname()
{
    static char name[9];
    int i;
    int r;
    name[0]='B';
    for(i=1; i<8; i++)
    {
        r=rand()%(26+10);
	if(r<10) name[i]=(char)(r+'0'); else name[i]=(char)(r+'A'-10);
    }
    name[8]=0;
    return name;
}

main(argc,OrigArgv)
int argc;
char **OrigArgv;
{
	FILE *in, *out;
    int c;
	char name[128];
    char *rn;
    char locname[12];
    char **argv;
	unsigned i;
    unsigned short owner,oldowner;
    time_t s;
    srand(time(&s));
    argv=OrigArgv;
    if(argc>1 && argv[1][0]=='-')
    {
      sscanf(argv[1]+1,"%hu",&owner);
      argv++;
      argc--;
    }
    else owner=65535;
    if(argc!=2 && argc!=3 && argc!=4 && argc!=5)
    {
        fprintf(stderr,"makewzd [-ownerid] filename [Title [(\"Description\" | @descfile) [iconfile]]]\n"
            "or: makewzd [-ownerid] filename @info\n\n"
            "  Creates filename.wzd from filename.bin, adding Title and\n"
            "  Description if specified, and loading in an iconfile if provided,\n"
            "  or using all data from the @info file (see docs\\makewzd.txt).\n"
            "  The default title is the filename.\n");
        return 1;
    }
    strcpy(name,argv[1]);
    strcat(name,".bin");
    if(!getlength(name) || NULL==(in=fopen(name,"rb")))
    {
		fprintf(stderr,"File `%s' is either unaccessible or zero length.\n",name);
		return 4;
    }
    strcpy(name,argv[1]);
    strcat(name,".wzd");
	if(NULL==(out=fopen(name,"wb")))
	{
		fprintf(stderr,"Cannot open `%s' for writing.\n",name);
		return 5;
	}
    if(argc==3 && argv[2][0]=='@')
    {
        FILE *config;
        char chars[81], icon_line[81];
        char *s=description;
        unsigned value,mask;
        int i,j;
        config=fopen(1+argv[2],"r");
        if(config==NULL)
        {
            fprintf(stderr,"Cannot open config file %s.\n");
            fclose(out);
            unlink(name);
            return 5;
        }
        fgets(title,MAX_TITLE,config);
        if(title[strlen(title)-1]=='\n') title[strlen(title)-1]=0;
        fgets(chars,80,config);
        for(i=0;i<12;i++)
        {
            fgets(icon_line,80,config);
            value=0;
            mask=1;
            for(j=0;j<12;j++)
            {
                if(icon_line[j]!=*chars)
                    value|=mask;
                mask<<=1;
            }
            icon[i*2+1]=value&0xff;
            icon[i*2+2]=value>>8;
        }
        while(-1!=(c=fgetc(config)) && s<description+MAX_DESC)
        {
           if(c=='\n')
           {
              c=fgetc(config);
              if(c=='\n')
              {
                 *s++='\r';
                 *s++='\n';
              }
              else if(c==-1)
                 break;
              else
              {
                 *s++=' ';
                 *s++=c;
              }
           }
           else *s++=c;
        }
        *s++='\r';
        *s++='\n';
        *s=0;
        fclose(config);
    }
    else
    {
        if(argc==2)
            strcpy(title,argv[1]);
        else strcpy(title,argv[2]);
        if(argc>=4 && argv[3][0]=='@')
        {
            FILE *desc=fopen(argv[3]+1,"r");
            char *s=description;
            if(desc==NULL)
            {
                fprintf(stderr,"Cannot open %s.\n",argv[3]+1);
                fclose(out);
                unlink(name);
                return 6;
            }
            while(-1!=(c=fgetc(desc)) && s<description+MAX_DESC-2)
            {
               if(c=='\n')
               {
                  c=fgetc(desc);
                  if(c=='\n')
                  {
                     *s++='\r';
                     *s++='\n';
                  }
                  else if(c==-1)
                     break;
                  else
                  {
                     *s++=' ';
                     *s++=c;
                  }
               }
               else *s++=c;
            }
            *s++='\r';
            *s++='\n';
            *s=0;
            fclose(desc);
        }
        else if(argc==2)
            strcpy(description,argv[1]);
        else if(argc==3)
            strcpy(description,argv[2]);
        else if(argc>=4)
            strcpy(description,argv[3]);
        if(argc==5)
        {
            FILE *iconfile=fopen(argv[4],"rb");
            char *s=icon+1;
            if(NULL==iconfile)
            {
                fprintf(stderr,"Cannot open icon file.\n");
                fclose(out);
                unlink(name);
                return 17;
            }
            getc(iconfile);
            while(-1!=(c=getc(iconfile)) && s<icon+sizeof(icon)/sizeof(char))
              *s++=c;
            while(s<icon+sizeof(icon)/sizeof(char)) *s++=0;
            fclose(iconfile);
        }
    }

	fprintf(out,"<SHARP WZD DATA>\r\n"
	"<DATA TYPE>\r\n"
	"MY PROGRAMS\r\n"
	"</DATA TYPE>\r\n"
	"<TITLE>\r\n"
	"%s\r\n"
	"</TITLE>\r\n"
	"<CATEGORY>\r\n"
	"PROGRAM\r\n"
	"</CATEGORY>\r\n"
    "<DESCRIPTION>\r\n"
    "%s\r\n"
	"</DESCRIPTION>\r\n"
	"<CONTENT>\r\n"
	"BIN_PROG_1\r\n"
	"</CONTENT>\r\n"
	"<DATA>\r\n"
    "PFILE:%s.BAS\r\n"
	"</DATA>\r\n"
    "<BIN>\r\n",
    title,
    description,
    rn=randname());
    for(i=0; i<sizeof(icon)/sizeof(char); i++) fputc(icon[i],out);
    for(i=0; i<0x14 && -1!=(c=fgetc(in)); i++)  fputc(c,out);
    if(c==-1)
    {
        fprintf(stderr,"Error reading!\n");
        fclose(out);
        unlink(name);
    }
    fread(locname,12,1,in);
    if(strcmp(locname,"BFILNAMEBAS"))
    {
        fprintf(stderr,"Warning: Old init code, no filename record.\n");
        fwrite(locname,12,1,out);
    }
    else
    {
        fwrite(rn,8,1,out);
        fwrite("BAS",4,1,out);
    }
    fread(&oldowner,2,1,in);
    if(oldowner!=65535)
    {
        fprintf(stderr,"Warning: No room for owner ID.\n");
        owner=oldowner;
    }
    fwrite(&owner,2,1,out);
    while(-1!=(c=fgetc(in))) fputc(c,out);
    fclose(in);
	fclose(out);
	return 0;
}




