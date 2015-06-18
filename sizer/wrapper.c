#include <stdio.h>
#include <string.h>

#include "table.c"

int charsize(unsigned char c, int *table)
{
    return table[(unsigned int)c];
}

void error()
{
    fprintf(stderr,"wrapper [font [width]]\n"
                   "  Wraps stdin text to stdout.\n"
                   "  Font can be pcnormal, oznormal, pclarge, ozlarge.\n"
                   "  Default font is pcnormal and default width is 239.\n"
                   "  Minimum width is 16.\n");
    exit(1);
}


main(int argc, char **argv)
{
    unsigned char word[256];
    char *p;
    int c;
    int width;
    int curpos=0;
    int wordstart=0;
    int *table;
    p=word;
    if(argc==2 || argc==3)
    {
      if(!stricmp(argv[1],"pcnormal"))
        table=table_pc_normal;
      else if(!stricmp(argv[1],"oznormal"))
        table=table_oz_normal;
      else if(!stricmp(argv[1],"pclarge"))
        table=table_pc_large;
      else if(!stricmp(argv[1],"ozlarge"))
        table=table_oz_large;
      else
        error();
    }
    else
    {
        if(argc!=1) error();
          else table=table_pc_normal;
    }
    if(argc==3)
    {
        width=atoi(argv[2]);
        if(width<16) error();
    }
    else width=239;
    while(-1!=(c=getchar()))
    {
            if(c=='\t') c=' ';
            if(c==' ' || c=='\n')
            {
                *p=0;
                printf("%s",word);
                wordstart=curpos;
                p=word;
                *p++=' ';
                curpos+=charsize(' ',table);
                if(c=='\n')
                {
                    if((c=getchar())=='\n')
                    {
                        printf("\n\n");
                        wordstart=curpos=0;
                        p=word;
                    }
                    else ungetc(c,stdin);
                }
            }
            else
            {
                curpos+=charsize(c,table);
                if(curpos>=width)
                {
                    if(wordstart)
                    {
                        curpos-=wordstart+charsize(' ',table);
                        *p=0;
                        p=word;
                        while(*p)
                        {
                            *p=p[1];  /* delete initial space */
                            p++;
                        }
                        putchar('\n');
                        p=word+strlen(word);
                        *p++=c;
                        wordstart=0;
                    }
                    else
                    {
                        *p=0;
                        printf("%s\n",word);
                        wordstart=0;
                        curpos=charsize(c,table);
                        p=word;
                        *p++=c;
                    }
                }
                else
                  *p++=c;
            }
    }
    *p=0;
    printf("%s\n",word);
}
