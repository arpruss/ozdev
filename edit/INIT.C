#include <oz.h>
#include "edit.h"

void exitsave(void);

extern byte modified0;
extern byte indexnum;

init()
{
    static unsigned fl;
/*    extern unsigned _ozautoofftime,_ozautoblanktime,_ozautolightofftime;
    _ozautoofftime=30;
    _ozautoblanktime=10;
    _ozautolightofftime=10; */
    preparehelp();
    ozsinglepage();
    ozrestorekeysettings();
    islocked=ozgetstatus()&OZ_STATUS_LOCKED;
    if(!ozgetmemoeditmode())
    {
        warn("There's a pending memo",
            "in the built-in memo editor.");
        return;
    }
    atexit(exitsave);
    if(ozreadauxword(16382)==0x6DBE && (fl=ozreadauxword(16378))<=16378
    && ozreadauxword(16380)==ozcrc16auxmem(16378-fl,fl))
    {
        file_length=16378;
        delete(0,16378-fl);
        wrapped_length=file_length=fl;
        clear_wrap_to_end(0);
        modified0=1;
        time_and_edit();
        modified0=0;
    }
    else
    {
        load_clip_permanent();
        goto_recnum=ozgetmemolastrecnum();
    }
    indexnum=ozgetindexby(TYPENUM_MEMO)-1;
    quick_index=0;
#ifndef FIXED
    switch(ozscancustomfonts())
    {
        case 0: NUM_FONTS=2; break;
        case 1: NUM_FONTS=3; font2=FONT_CUSTOM0; break;
        case 2: NUM_FONTS=3; font2=FONT_CUSTOM1; break;
        case 3: NUM_FONTS=4; font2=FONT_CUSTOM0; font3=FONT_CUSTOM1; break;
    }
    switch(ozgetappfont())
    {
        case 0: font=1; break;
        case 1: font=0; break;
        case FONT_CUSTOM0:
                font=2;
                break;
        case FONT_CUSTOM1:
                if(NUM_FONTS==4) font=3;
                    else font=2;
                break;
    }
    if(font>=NUM_FONTS) font=0;
#endif
}

