#include <oz.h>
#include <ozmemo.h>
#include "same.h"

#define VERSION '2'
#define HEADER 43

void load_stuff(void)
{
    static int version;
    if((version=ozopendatamemoread("zz:same!.data"))<0)
        return;
    if(version!=VERSION)
    {
        ozputs(0,10,"Incompatible version zz:same!.data file.");
        ozputs(0,20,"Press ESC to exit, any other key to delete.");
        switch(ozgetch())
        {
            case 27:
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
                exit(0);
        }
        ozunlink(TYPENUM_MEMO,ozgetdatamemorecnum());
        return;
    }
    if(ozreaddatamemo(highscores,sizeof(highscores)) < sizeof(highscores))
    {
        memset(highscores,sizeof(highscores),0);
        return;
    }
    if(ozreaddatamemo(highscore_names,sizeof(highscore_names))
      < sizeof(highscore_names))
    {
        memset(highscore_names,sizeof(highscore_names),0);
        memset(highscores,sizeof(highscores),0);
    }
    if(ozreaddatamemo(&current_size,1)==1)
     if(ozreaddatamemo(&current_number,1)==1)
      if(ozreaddatamemo(&current_slow,1)==1)
        ozreaddatamemo(&current_quiet,1);
    if(current_size>=NUMSIZES) current_size=DEF_SIZE;
    if(current_number>=PIECESRANGE) current_number=DEF_NUMBER;
    if(current_slow>1) current_slow=DEF_SLOW;
    if(current_quiet>1) current_quiet=DEF_QUIET;
    ozclick(!current_quiet);
    ozclosedatamemoread();
}

void save_stuff(void)
{
    if(!changed) return;
    ozsetfont(FONT_OZ_NORMAL);
    _ozfilledbox(INFO_X,0,239-INFO_X,10,0);
    ozputs(INFO_X,0,"Saving...");
    ozsetpowerkeyhandling(0);
    ozopendatamemowrite("zz:same!.data",'2');
    ozwritedatamemo(highscores,sizeof(highscores));
    ozwritedatamemo(highscore_names,sizeof(highscore_names));
    ozwritedatamemo(&current_size,1);
    ozwritedatamemo(&current_number,1);
    ozwritedatamemo(&current_slow,1);
    ozwritedatamemo(&current_quiet,1);
    ozclosedatamemowrite();
}

