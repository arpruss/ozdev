#include <stdio.h>

main(int argc, char **argv)
{
    char line[1024];
    FILE *f;
    int c;
    long count;
    f=fopen(argv[1],"rb");
    count=0;
    while(-1!=(c=fgetc(f)))
    {
        if(count) putchar(',');
        if(!(count%16)) putchar('\n');
        count++;
        printf("0x%X",c);
    }
}
