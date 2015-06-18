#include <stdio.h>

#include "table.c"

int width=239;

int getsize(unsigned char *s,int *table)
{
    register int sum=0;
    while(*s) sum+=table[(unsigned int)(*s++)];
    return sum;
}


sizeit(unsigned char *s,int *table,char *fontname)
{
    printf("%s: width=%d; for center, x=%d; for right align, x=%d\n",
        fontname,
        getsize(s,table),(width+1)/2-getsize(s,table)/2,width-getsize(s,table));
}

main(int argc, char **argv)
{
    unsigned char string[256];
    if(argc==2) width=atoi(argv[1]);
    while(NULL!=gets(string))
    {
        sizeit(string,table_pc_normal,"FONT_PC_NORMAL");
        sizeit(string,table_oz_normal,"FONT_OZ_NORMAL");
        sizeit(string,table_pc_large,"FONT_PC_LARGE");
        sizeit(string,table_oz_large,"FONT_OZ_LARGE");
    }
}
