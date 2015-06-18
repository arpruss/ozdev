#include <oz.h>
#include "edit.h"

char directory[]="edit/";

int write_file(void)
{
    static unsigned pos,size;
    ozcls();
    ozputs0(0,"Writing...");
    if(open1(filename,O_WRONLY)<0)
    {
        ozcls();
        ozputs0(0,"Error opening file!");
        ozgetch();
        return -1;
    }
    pos=0;
    clear_wrap_to_end(0);
    while(pos<file_length)
    {
        if(pos+LINE_BUF_LEN<=file_length) size=LINE_BUF_LEN;
          else size=file_length-pos;
        ozreadauxmem(pos,line_buf,size);
        if(write1(line_buf,size)<size)
        {
            ozcls();
            ozputs0(0,"Error writing!");
            close1();
            return -1;
        }
        pos+=size;
    }
    close1();
}


void read_file(void)
{
    static int r;
    static byte i;
    static byte b;
    ozcls();
    ozputs0(0,"Reading...");
    file_length=0;
    if(open1(filename,O_RDONLY)<0)
    {
        ozputs0(20,"Cannot open!  Create new? [Y/n]");
        if(ozgetch()=='n') exit(0);
        return;
    }
    do
    {
        r=read1(line_buf,LINE_BUF_LEN);
        if(file_length+r>MAX_FILE_LENGTH)
        {
            ozcls();
            ozputs0(0,"File too long!");
            ozputs0(10,"Truncate? [y/N]");
            if(ozgetch()!='y') exit(0);
            r=MAX_FILE_LENGTH-file_length;
        }
        for(i=0;i<r;i++)
        {
            b=line_buf[i];
            b&=0x7f;
            if(b<32) b='*';
            line_buf[i]=b;
        }
        if(r>0)
        {
            ozwriteauxmem(file_length,line_buf,r);
            file_length+=r;
        }
    } while(r==LINE_BUF_LEN && file_length<MAX_FILE_LENGTH);
     /* note: LINE_BUF_LEN does not divide MAX_FILE_LENGTH, and this
        is important for truncate message to work */
    close1();
}

main()
{
    ozsinglepage();
    menu();
}

