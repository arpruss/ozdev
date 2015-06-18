#include <stdlib.h>

main()
{
    float f;
    char *buf="12.2";
    sscanf(buf,"%f",&f);
    f*=2;

}
