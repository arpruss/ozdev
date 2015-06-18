#include <oz.h>

void ozdelay64hz(unsigned len)
{
    static unsigned start_timer;
    static byte tstate;
    tstate=ozget64hztimerstate();
    oz64hztimeron();
    start_timer=_oz64hz_word;
    while((unsigned)(_oz64hz_word-start_timer)<len) ozintwait();
    if(!tstate) oz64hztimeroff();
}


