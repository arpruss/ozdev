#include <oz.h>

byte image[2400]="<-image starts here";

main()
{
    static unsigned i;
    register byte *p=image;
    for(i=0;i<2400;i++) ozdisplayputbyte(i,*p++);
    ozgetch();
}
