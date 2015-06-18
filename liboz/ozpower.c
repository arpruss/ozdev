#include <oz.h>

extern byte _ozdisablepowerkey;

void ozsetpowerkeyhandling(byte state)
{
   _ozdisablepowerkey=!state;
}
