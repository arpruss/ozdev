#include <oz.h>

static unsigned long time0(void)
{
/*
   this is a fake time since 1970 which has the property that it is
   incremented by AT LEAST 1 every second and is faster to compute
   than the real time
 */
   return ozsec()+86400L*ozday()
    + (86400L*31)*ozmonth()
    + (86400L*12*31)*(ozyear()-1970)
    + ozhour()*3600L+(unsigned int)ozmin()*60;
}

unsigned long oztime_fake(void)
{
    static unsigned long t0;
    t0=time0();
    if(t0!=time0()) return time0(); else return t0;
        /* watch out for carry problems on tc8521 */
}

