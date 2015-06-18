#include <oz.h>

unsigned ozkeydelay64hz(unsigned len)
{
    static unsigned start_timer;
    static byte tstate;
    tstate=ozget64hztimerstate();
    oz64hztimeron();
    start_timer=_oz64hz_word;
    while((unsigned)(_oz64hz_word-start_timer)<len && !ozkeyhit()) ozintwait();
    if(!tstate) oz64hztimeroff();
    if(ozkeyhit()) return ozgetch(); else return 0;
}

