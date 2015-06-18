#include <stdio.h>
#include "help.xbm"
#include "smain.xbm"
#include "mailedit.xbm"

int aftereol;

void dumpbin(unsigned char c,FILE *f)
{
    static int i=0;
    if(aftereol)
    {
        fprintf(f,"\ndefb ");
        aftereol=0;
    }
    else
        fputc(',',f);
    fprintf(f,"%u",(unsigned int)c);
    i++;
    if(i%16==0) aftereol=1;
}

main(int argc, char **argv)
{
    FILE *f;
    unsigned char *data_bits;
    int pos;
    int i;
    aftereol=1;
    if(argc==1)
    {
        data_bits=help_bits;
        f=fopen("help.inc","w");
    }
    else if(argv[1][0]=='s')
    {
        data_bits=smain_bits;
        f=fopen("smain.inc","w");
    }
    else
    {
        printf("(MailEdit)\n");
        data_bits=mailedit_bits;
        f=fopen("mailedit.inc","w");
    }
    for(pos=i=0;i<1957;i++,pos++)
    {
        dumpbin(data_bits[pos],f);
    }
    fprintf(f,"\n$auxbuffer:\n$copybuffer:");
    aftereol=1;
    for(i=0;i<256;i++,pos++)
    {
        dumpbin(data_bits[pos],f);
    }
    fprintf(f,"\n_iloc_and_wt:");
    aftereol=1;
    for(i=0;i<256;i++,pos++)
    {
        if(pos<2400) dumpbin(data_bits[pos],f);
            else dumpbin(0,f);
    }
    fprintf(f,"\n");
    fclose(f);
    return 0;
}

