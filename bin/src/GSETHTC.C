#include <string.h>
#include <stdio.h>

main(int argc, char **argv)
{
    FILE *f;
    int i;
    char path0[1024],path[1024];
    strcpy(path0,argv[0]);
    for(i=strlen(path0)-1;i>=0;i--)
      if(path0[i]=='\\')
        break;
    path0[i]=0;
    strcpy(path,path0);
    strcat(path,"\\..\\bin\\sethtc.bat");
    f=fopen(path,"w");
    if(NULL==f)
    {
        fprintf(stderr,"Error opening %s\n",path);
        return 3;
    }
    fprintf(f,"@echo off\n"
              "set L=%s\\..\\compiler\\",path0);
    return 0;
}
