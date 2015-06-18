#include <oz.h>

void serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

void dump_table(char *name,int font)
{
    static char buffer[64];
    static int i;
    ozfont(font);
    sprintf(buffer,"int table_%s[256]= {\r\n",name);
    serputs(buffer);
    for(i=0;i<256;i++)
    {
        sprintf(buffer,"%d",ozputch(0,-1,i));
        serputs(buffer);
        if(i!=255) ozserialout(',');
        if(i%16==15) serputs("\r\n");
    }
    serputs("};\r\n\r\n");
}

main()
{
    ozsetbaud(BAUD9600);
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    dump_table("pc_normal",FONT_PC_NORMAL);
    dump_table("oz_normal",FONT_OZ_NORMAL);
    dump_table("pc_large",FONT_PC_LARGE);
    dump_table("oz_large",FONT_OZ_LARGE);
}
