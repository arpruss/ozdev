#include <string.h>
#include <stdio.h>

void send(FILE *f, char *psect, int start)
{
    char line[1024];
    int inpsect;
    inpsect=start;
    fseek(f,0,SEEK_SET);
    printf("psect %s",psect);
    while(NULL!=fgets(line,1023,f))
    {
        if(!strncmp(line,"psect",5))
        {
            if(strcmp(line+6,psect))
              inpsect=0;
            else
              inpsect=1;
        }
        else if(inpsect) printf("%s",line);
    }
}


int main(int argc, char **argv)
{
    FILE *f;
    if(argc!=2 || NULL == (f=fopen(argv[1],"r")))
    {
        fprintf(stderr,"segord filename");
        return 2;
    }
    send(f,"data\n",1);
    send(f,"bss\n",0);
    send(f,"text\n",0);
    return 0;
}
