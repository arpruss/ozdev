#include <ozinput.h>

unsigned ozngetch(void)
{
    static unsigned c;
    for(;;)
    {
        switch(c=ozgetch())
        {
            case KEY_POWER:
            case KEY_MAIN:
            case KEY_TELEPHONE:
            case KEY_MEMO:
            case KEY_SCHEDULE:
                ozexitto(c);
            case KEY_MYPROGRAMS:
                exit(0);
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            default:
                return c;
        }
    }
}

int oznkeyhit(void)
{
    static unsigned c;
    for(;;)
    {
      if(!ozkeyhit()) return 0;
      c=ozgetch();
      switch(c)
        {
            case KEY_POWER:
            case KEY_MAIN:
            case KEY_TELEPHONE:
            case KEY_SCHEDULE:
                ozexitto(c);
            case KEY_MYPROGRAMS:
                exit(0);
            case KEY_BACKLIGHT:
                oztogglelight();
                break;
            default:
                ozungetch(c);
                return 1;
        }
    }
}


