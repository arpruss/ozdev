#include <oz.h>
#if 0
#include <ozmenu.h>
#endif
#include "same.h"

byte current_size=DEF_SIZE;
byte current_number=DEF_NUMBER;
byte current_slow=DEF_SLOW;
byte current_quiet=DEF_QUIET;

struct ozmenuentry main_menu[7]=
{
    {'p',"Play!"},
    {'s',"Size [__x__]"},
    {'n',"Number of pieces [_]"},
    {'f',NULL},
    {'q',NULL},
    {'h',"Help"},
    {'e',"Exit"}
};

struct ozmenuentry number_menu[PIECESRANGE]=
{
    {'2',"Two types of pieces"},
    {'3',"Three types of pieces"},
    {'4',"Four types of pieces"},
    {'5',"Five types of pieces"}
};

struct ozmenuentry size_menu[NUMSIZES]=
{
    {'l',"Large (24x10)"},
    {'n',"Normal (20x10)"},
    {'m',"Medium (12x10)"},
    {'s',"Small square (10x10)"},
    {'t',"Tiny (5x5)"}
};

byte sizes[][2]={
    {24,10},
    {20,10},
    {12,10},
    {10,10},
    {5,5}
};

int menu(void)
{
    static int sel;
    width=sizes[current_size][0];
    height=sizes[current_size][1];
    numpieces=current_number+2;
    while(1)
    {
        showhigh();
        sprintf(main_menu[1].label,"Size [%dx%d]",width,height);
        sprintf(main_menu[2].label,"Number of pieces [%d]",numpieces);
        if(current_slow) main_menu[3].label="Fast mode toggle [now off]";
          else main_menu[3].label="Fast mode toggle [now on]";
        if(current_quiet) main_menu[4].label="Quiet mode toggle [now on]";
           else main_menu[4].label="Quiet mode toggle [now off]";
        sel=ozmenu(0,0,"Same!",0,7,main_menu,0);
        switch(sel)
        {
            case 0: return 1;
            case 1: if((sel=ozmenu(0,0,"Size",current_size,NUMSIZES,size_menu,0))>=0)
                    {
                        if(current_size!=sel)
                        {
                            current_size=sel;
                            width=sizes[current_size][0];
                            height=sizes[current_size][1];
                            changed=1;
                        }
                    }
                    break;
            case 2: if((sel=ozmenu(0,0,"Number of pieces",
                    current_number,PIECESRANGE,number_menu,0))>=0)
                    {
                        if(current_number!=sel)
                        {
                            current_number=sel;
                            numpieces=current_number+2;
                            changed=1;
                        }
                    }
                    break;
            case 3: current_slow=!current_slow;
                    changed=1; break;
            case 4: ozclick(current_quiet);
                    current_quiet=!current_quiet;
                    changed=1;
                    break;
            case 5: help(); break;
            default:
                    return 0;
        }
    }
}

