#include <stdio.h>
#include <string.h>

main(int argc, char **argv)
{
    int i;
    unsigned len;
    FILE *f;
    len=0;
    if(argc<2 || NULL==(f=fopen(argv[1],"w")))
    {
        fprintf(stderr,"zxecho outfile [args]");
        return 4;
    }
    for(i=2;i<argc;i++)
    {
        fputs(argv[i],f);
        len+=strlen(argv[i])+1;
        if(i!=argc-1)
        {
            if(len+strlen(argv[i+1])>75)
            {
                fputs(" \\\n",f);
                len=0;
            }
            else fputc(' ',f);
        }
    }
    fputc('\n',f);
    return 0;
}
