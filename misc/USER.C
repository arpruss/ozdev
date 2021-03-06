#include <stdlib.h>
#include <string.h>

close(){}

struct ozschedule_header
{
    char header[13];
    char date[4+2+2+1];
    char time[2+2+2+2+1];
    char blanks[4+1];
};

void *
makescheduleentry(char *date, char *time, char *entry, unsigned *len)
{
    int l=strlen(entry)+1;
    struct ozschedule_header *buf=malloc(*len=sizeof(struct ozschedule_header)+l);
    if(buf==NULL) return NULL;
    strcpy(buf->date,date);
    strcpy(buf->time,time);
    strcpy(buf->blanks,"    ");
    memcpy(&(buf[1]),entry,l);
    return buf;
}

char *makeuserfile(char *n, char *f, unsigned *len, char *progname)
{
    extern char *_ozfilename;
    int i,sl;
/*
    char *buf=malloc(*len+13+12+12+6);
    memset(buf,0,13);
    sl=strlen(n);
    for(i=0;i<8;i++)
        buf[13+i]=i<sl?n[i]:' ';
    strcpy(buf+13+8,"DAT");
    strcpy(buf+13+8+4,_ozfilename);
    strcpy(buf+13+8+4+8,"BAS");
    strcpy(buf+13+8+4+8+4,"SHARP");
    memcpy(buf+13+8+4+8+4+6,f,*len);
    *len+=13+8+4+8+4+6;
*/

    char *buf="\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "FILENAMEDAT\0"
        "FILENAMEBAS\0"
        "SHARP\0Testing!\0";
     memcpy(buf+13+8+4,progname,8);
     *len=13+12+12+6+9;


/*
    buf="\0\0\0\0\0\0\0\0\0\0\0\0\0"
        "20000207\0"
        "This is the title\0\0\0\0"
        "BODY\0";
    *len=13+8+1+21+5; */
    return buf;
}

main(int argc, char **argv)
{
    unsigned id=0;
    unsigned len;
    unsigned r;
    char *entry;
    char buf[20];
    entry=makescheduleentry("20000709","01231234","This is a schedule "
    "entry generated by user.c.",&len);
    r=(char)ozwritefile(entry,len,3/* schedule? */,&id);
/*
    char *f="This is a test of the file system...  321...";
    ozputs(0,0,argv[0]);
    len=strlen(f)+1;
    f=makeuserfile(f,"Test1",&len,argv[0]);
    if(f==NULL)
    {
        ozputs(10,10,"ERROR!");
        ozgetch();
        return;
    }
    r=(char)ozwritefile(f,len,7,&id);
*/
    sprintf(buf,"%x (%d) %x",id,len,r);
    ozputs(10,10,buf);
    ozgetch();
}

