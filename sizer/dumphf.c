#include <oz.h>

void serputs(char *s)
{
    while(*s) ozserialout(*s++);
}

main()
{
    static char buffer[64];
    static int i,j,k;
    static byte v;
    ozsetbaud(BAUD9600);
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    for(i=0;i<256;i++)
    {
        serputs("{ ");
        ozcls();
        sprintf(buffer,"%d",ozputch(0,0,i));
        serputs(buffer);
        for(j=0;j<8;j++)
        {
            ozserialout(',');
            v=0;
            for(k=0;k<8;k++) v=(v<<1)|ozgetpoint(j,k);
            ozdisplayputbyte(15+30*j,v);
            sprintf(buffer,"0x%x",v);
            serputs(buffer);
        }
        serputs(" }");
        if(i!=255) serputs(",\r\n");
    }
}

