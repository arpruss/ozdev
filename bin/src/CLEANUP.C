#include <stdio.h>

main(int argc, char **argv)
{
    FILE *f;
    int c;
    if(argc!=2) return 2;
    f=fopen(argv[1],"rb");
    if(f==NULL) return 2;
    while(-1!=(c=fgetc(f)))
    {
        switch(c)
        {
            case 0xa: putchar('\n'); break;
            case 0xd: break;
            default: putchar(c);
        }
    }
    return 0;
}
