#include <oz.h>
#include "file.h"

extern unsigned _ozfileowner;

static unsigned atou2digit(char *s)
{
    return 10*s[0]+s[1]-('0'+10*'0');
}

void ozsetowner(unsigned o)
{
    _ozfileowner=o;
}

unsigned ozgetowner(void)
{
    return _ozfileowner;
}

unsigned __ozfileheaderdecode(unsigned *length_ptr)
{
    static unsigned owner_id,length;
    length=(header0.fake_time)[1]-'0';
    length=24*length+atou2digit(header0.fake_alarm);
    length=60*length+atou2digit((header0.fake_alarm)+2);
    *length_ptr=length;
    owner_id=(header0.fake_time)[0]-'0';
    owner_id=60*owner_id+atou2digit((header0.fake_time)+2);
    owner_id=10*owner_id+atou2digit((header0.fake_time)+4)-14;
    owner_id=60*owner_id+atou2digit((header0.fake_time)+6);
    return owner_id;
}

