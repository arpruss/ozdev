#include <oz.h>

extern byte _ozdisablepowerkey;

byte ozgetpowerkeyhandling(void)
{
  return !_ozdisablepowerkey;
}


