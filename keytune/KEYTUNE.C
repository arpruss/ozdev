#include <string.h>
#include <stdlib.h>
#include <oz.h>


struct ozmenuentry main_menu[]=
{
    {'k',"(K)eyboard speed settings"},
    {'b',"(B)lanking and power-off settings"},
    {'c',NULL}, /* click */
    {'s',NULL}, /* sound */
    {'f',NULL}, /* full page scrolling */
    {'d',"restore all (D)efaults"},
    {'u',"back(U)p settings to flash"},
    {'r',"(R)estore settings from flash"},
    {KEY_DELETE,"DEL: delete backup of settings"},
    {'q',"(Q)uit"}
};



enum
{
    KEY_SPEED=0, BLANKING, CLICK, SOUND, FULLSCROLL, DEFAULT, BACKUP,
    RESTORE, DELSET, QUIT
};

#define NUM_MENU (sizeof main_menu/sizeof *main_menu)

static char buffer[80];

char buffer_light[80];
char buffer_blank[80];
char buffer_off[80];

struct ozmenuentry blanking_menu[]=
{
    {'l',buffer_light},
    {'b',buffer_blank},
    {'o',buffer_off},
    {'e',NULL},
    {'q',"(Q)uit to main menu"}
};



void putline(byte y,char *s)
{
    static byte x;
    x=ozputs(0,y,s);
    _ozfilledbox(x,y,239-x,ozgetfontheight(ozgetfont()),0);
}

void show(void)
{
    sprintf(buffer,"Delay between repeats: %d, initial delay: %d, click: o%s",
       ozgetrepeatspeed(),ozgetrepeatdelay(),_ozclick_setting?"n":"ff");
    putline(56,buffer);
}

void keyspeed(void)
{
    static byte x=0;
    static unsigned key;
    static byte value;
    ozcls();
    ozfont(FONT_OZ_NORMAL);
    ozputs(0,0,"PAGE-UP: Increase key delay between repeats");
    ozputs(0,8,"PAGE-DOWN: Decrease key delay between repeats");
    ozputs(0,16,"UP: Increase initial key repeat delay");
    ozputs(0,24,"DOWN: Decrease initial key repeat delay");
    ozputs(0,32,"-: Toggle key click, DEL: Restore defaults");
    ozputs(0,40,"ESC: Abort, ENTER: Save");
    ozputs(0,48,"Other keys: test repeat");
    show();
    for(;;)
    {
        key=ozgetch();
        switch(key)
        {
            case KEY_PAGEUP:
                value=ozgetrepeatspeed();
                if(value<255) value++;
                ozsetrepeatspeed(value);
                show();
                break;
            case KEY_UP:
                value=ozgetrepeatdelay();
                if(value<255) value++;
                ozsetrepeatdelay(value);
                show();
                break;
            case KEY_PAGEDOWN:
                value=ozgetrepeatspeed();
                if(value>0) value--;
                ozsetrepeatspeed(value);
                show();
                break;
            case KEY_DOWN:
                value=ozgetrepeatdelay();
                if(value>0) value--;
                ozsetrepeatdelay(value);
                show();
                break;
            case 8:
                _ozclick_setting=1;
                ozsetrepeatdelay(32);
                ozsetrepeatspeed(5);
                show();
                break;
            case '-':
                _ozclick_setting=!_ozclick_setting;
                show();
                break;
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
                return;
            case KEY_LOWER_ENTER:
            case KEY_UPPER_ENTER:
            case 13:
                ozsavekeysettings();
                ozdelay64hz(8);
                return;
            default:
                if(ozputch(x,-1,key)>239)
                {
                    x=0;
                    _ozfilledbox(0,72,239,8,0);
                }
                x=ozputch(x,72,key);
                break;
        }
    }
}


char *value(unsigned v)
{
    if(v==0) return("OFF");
    if(v==3600) return "1 hr";
    if(v/3600*3600==v)
    {
        sprintf(buffer,"%u hrs",v/3600);
        return buffer;
    }
    if(v/60*60==v)
    {
        sprintf(buffer,"%u min",v/60);
        return buffer;
    }
    sprintf(buffer,"%u sec",v);
    return buffer;
}

void editvalue(char *s,unsigned *vptr)
{
    char edit[20];
    strcpy(edit,value(*vptr));
    ozcls();
    putline(0,s);
    putline(20,"Indicate units with 'h', 'm' or 's' after value.");
    putline(30,"If no units indicated, assumed to be seconds.");
    putline(40,"Put 'off' or '0' to disable.");
    if(ozeditline(0,10,edit,40,239)<=0) return;
    if(NULL!=strchr(edit,'o'))
    {
        *vptr=0;
        return;
    }
    *vptr=atoi(edit);
    if(NULL!=strchr(edit,'m'))
    {
        *vptr *=60;
        return;
    }
    if(NULL!=strchr(edit,'h'))
    {
        *vptr *= 3600;
        return;
    }
}

void blanking(void)
{
    static int sel=0;
    for(;;)
    {
        sprintf(buffer_light,"Back(L)ight auto off: %s",value(_ozautolightofftime));
        sprintf(buffer_blank,"(B)lanking: %s",value(_ozautoblanktime));
        sprintf(buffer_off,"Auto (O)ff time: %s",value(_ozautoofftime));
        blanking_menu[3].label=(_ozprogoptions & (byte)OZ_OPTION_KEY_AFTER_BLANK)?
            "(E)xtra key to restore after blanking [on]"
          : "(E)xtra key to restore after blanking [off]";
        switch(ozmenu(0,0,"Blanking settings",sel,sizeof blanking_menu/sizeof *blanking_menu,blanking_menu,0))
        {
            case 0:
                editvalue("Backlight auto off time",&_ozautolightofftime);
                ozsavekeysettings();
                break;
            case 1:
                editvalue("Blanking time",&_ozautoblanktime);
                ozsavekeysettings();
                break;
            case 2:
                editvalue("Auto off time",&_ozautoofftime);
                ozsavekeysettings();
                break;
            case 3:
                _ozprogoptions ^= OZ_OPTION_KEY_AFTER_BLANK;
                ozsavekeysettings();
                break;
            default:
                return;
        }
    }
}





main()
{
    static byte defaults[SETTINGS_LENGTH];
    static byte cursettings[SETTINGS_LENGTH];
    static int sel=0;
    ozcopyfromcursettings(defaults);
    ozrestorekeysettings();
    for(;;)
    {
        ozcls();
        if(_ozclick_setting) main_menu[CLICK].label="(C)lick [now on]";
            else main_menu[CLICK].label="(C)lick [now off]";
        if(_ozprogoptions & (byte)OZ_OPTION_NO_SOUND)
            main_menu[SOUND].label="(S)ound [now off]";
        else
            main_menu[SOUND].label="(S)ound [now on]";
        if(_ozprogoptions & (byte)OZ_OPTION_SCROLL_FULL)
            main_menu[FULLSCROLL].label="(F)ull page scrolling [now on]";
        else
            main_menu[FULLSCROLL].label="(F)ull page scrolling [now off]";
        switch(sel=ozmenu(0,0,"Keytune 3",sel,NUM_MENU,main_menu,0))
        {
            case KEY_SPEED:
                keyspeed();
                break;
            case BLANKING:
                blanking();
                break;
            case CLICK:
                _ozclick_setting=!_ozclick_setting;
                ozsavekeysettings();
                break;
            case SOUND:
                _ozprogoptions ^= (byte)OZ_OPTION_NO_SOUND;
                ozsavekeysettings();
                break;
            case FULLSCROLL:
                _ozprogoptions ^= (byte)OZ_OPTION_SCROLL_FULL;
                ozsavekeysettings();
                break;
            case BACKUP:
                if(open1(".keytunerc",O_WRONLY)<0) break;
                ozcopyfromcursettings(cursettings);
                write1(cursettings,SETTINGS_LENGTH);
                close1();
                break;
            case RESTORE:
                if(open1(".keytunerc",O_RDONLY)>=0 &&
                   read1(cursettings,SETTINGS_LENGTH)==SETTINGS_LENGTH)
                {
                    ozcopytocursettings(cursettings);
                    ozsavekeysettings();
                }
                close1();
                break;
            case DELSET:
                unlink(".keytunerc");
                break;
            case DEFAULT:
                ozcopytocursettings(defaults);
                ozsavekeysettings();
                break;
            default:
                return;
        }
    }
}


