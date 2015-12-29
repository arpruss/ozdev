#include <string.h>
#include <stdio.h>

main()
{
    char line[16384];
    char c;
    int i,len;
    while(NULL!=gets(line))
    {
        len=strlen(line);
        for(i=0;i<len/2;i++)
        {
            c=line[len-1-i];
            line[len-1-i]=line[i];
            line[i]=c;
        }
        puts(line);
    }
    return 0;
}
