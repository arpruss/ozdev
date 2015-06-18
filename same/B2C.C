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
    int start=1,firstline=0;
    char word1[128],word2[128];
    while(1==scanf("%s",word1))
    {
        if(word1[0]=='>')
        {
            if(!start) printf("\n};\n");
            printf("byte %s[]={\n",word1+1);
            firstline=1;
            start=0;
        }
        else
        {
            scanf("%s",word2);
            if(!firstline) printf(",\n");
            firstline=0;
            printf(" 0x%02X,0x%02X /* %s %s */",b2i(word1),b2i(word2),word1,word2);
        }
    }
    printf("\n};\n");
    return 0;
}
