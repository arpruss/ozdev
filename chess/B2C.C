#include <stdio.h>

unsigned char b2i(char *s)
{
    unsigned char mask=1;
    unsigned char ret=0;
    unsigned int i;
    for(i=0;i<8;i++,mask<<=1)
        if(s[i]!='.') ret|=mask;
    return ret;
}

main()
{
    char word1[128];
    int new=1;
    while(1==scanf("%s",word1))
    {
        if(*word1=='!')
        {
            putchar('\n');
            return 0;
        }
        if(*word1=='-')
        {
            printf(" ;%s\n",word1+1);
            new=1;
            continue;
        }
        if(new) printf("defb "); else putchar(',');
        new=0;
        printf("0%02Xh",b2i(word1));
    }
    return 0;
}
