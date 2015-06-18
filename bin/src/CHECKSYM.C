#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN 1024

main(int argc, char **argv)
{
    int violations=0;
    char line[LINELEN];
    char symbol[2][256];
    char segment[2][256];
    unsigned value[2];
    int found;
    int i;
    int j;
    FILE *f=stdin;
    if((argc%3 != 2 || argc<=2) || NULL==(f=fopen(argv[1],"r")))
    {
        if(f==NULL)
        fprintf(stderr,"Cannot open %s.\n",argv[1]);
        fprintf(stderr,
        "checksym mapfile symbol relation hex-value [...]\n"
        "  Checks whether the location of the specified symbols\n"
        "  in the mapfile satisfies the specified inequality.  The\n"
        "  possible values for `relation' are `eq', `lt', `gt', `le' and `ge',\n"
        "  meaning =, <, >, <= and >=, respectively, as well as `exists'\n"
        "  which counts as satisfied if hex-value is 1 and the symbol exists\n"
        "  or if hex-value is 0 and the symbol fails to exist, and `print'\n"
        "  which is always satisfied and the hex-value is ignored (but must\n"
        "  be specified) and which prints the symbol.\n"
        "  For instance,\n"
        "    checksym prog.map __Hlowpage lt a000\n"
        "  checks to ensure that the symbol __Hlowpage (end of psect\n"
        "  lowpage) is located below address a000h, as indeed it\n"
        "  is supposed to be on the standard Hi-Tech C SDK convention.\n"
        "  As results, checksym lists the violations and its return\n"
        "  code is the number of violations or 254, whichever is lesser.\n"
        "  Except in the case of `symbol exists 1', a missing symbol does\n"
        "  not yield a violation\n");
        return 255;
    }
    found=0;
    while(NULL!=fgets(line,LINELEN,f))
    {
        if(!strcmp(line,
          "                                  Symbol Table\n"))
        {
            found=1;
            break;
        }

    }
    if(!found)
    {
        fprintf(stderr,"%s: Symbol table not found\n",argv[1]);
        return 255;
    }
    while(NULL!=fgets(line,LINELEN,f))
    {
        segment[0][0]=symbol[0][0]=0;
        segment[1][0]=symbol[1][0]=0;
        sscanf(line,"%s %s %x %s %s %x",symbol[0],segment[0],value,
            symbol[1],segment[1],value+1);
        for(i=0;i<2;i++)
        {
            if(symbol[i][0]==0 || segment[i][0]==0) break;
/*            printf("%s %s %x\n",symbol[i],segment[i],value[i]); */
            for(j=2;j<argc;j+=3)
            {
                if(!strcmp(symbol[i],argv[j]))
                {
                    unsigned v;
                    int violated=1;
                    sscanf(argv[j+2],"%x",&v);
                    if(!strcmp(argv[j+1],"eq"))
                    {
                        if(value[i]==v) violated=0;
                    }
                    else if(!strcmp(argv[j+1],"lt"))
                    {
                        if(value[i]<v) violated=0;
                    }
                    else if(!strcmp(argv[j+1],"le"))
                    {
                        if(value[i]<=v) violated=0;
                    }
                    else if(!strcmp(argv[j+1],"gt"))
                    {
                        if(value[i]>v) violated=0;
                    }
                    else if(!strcmp(argv[j+1],"ge"))
                    {
                        if(value[i]>=v) violated=0;
                    }
                    else if(!strcmp(argv[j+1],"true"))
                        violated=0;
                    else if(!strcmp(argv[j+1],"exists"))
                    {
                        if(v)
                        {
                            violated=0;
                            strcpy(argv[j+1],"true");
                        }
                    }
                    else if(!strcmp(argv[j+1],"print"))
                    {
                        printf("%s=%04X\n",argv[j],value[i]);
                        violated=0;
                    }
                    if(violated)
                    {
                        violations++;
                        printf("%s: violation of %s %s %s (%s=%04x)\n",
                          argv[1],argv[j],argv[j+1],argv[j+2],symbol[i],
                          value[i]);
                    }
                }
            }
        }
    }
    for(j=2;j<argc;j+=3)
        if(!strcmp(argv[j+1],"exists"))
        {
            unsigned v;
            sscanf(argv[j+2],"%x",&v);
            if(v)
            {
                violations++;
                printf("%s: violation of %s %s %s\n",
                  argv[1],argv[j],argv[j+1],argv[j+2]);
            }
        }
    return violations<255?violations:254;
}

