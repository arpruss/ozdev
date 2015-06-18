#include <oz.h>

byte ozgetfontheight(byte f)
{
    return (_ozfontpointers[f])[6];
}

