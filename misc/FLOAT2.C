#include <stdlib.h>

static s()
{
    sin();
}

main()
{
    static char buf[]="123.344 4a4";
    static char obuf[64];
    float a;
    int b;
    sscanf(buf,"%f %x",&a,&b);
    sprintf(obuf,"v1=%f, v2=%x",a,b);
    ozputs(10,10,obuf);
    a=123.344;
    b=0x4a4;
    sprintf(obuf,"v1=%.2f, v2=%x",a,b);
    ozputs(20,20,obuf);
    ozgetch();
}
