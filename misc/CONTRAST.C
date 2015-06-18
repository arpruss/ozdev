#include <oz.h>

main()
{
    char buf[45];
    byte contr;
    contr=ozgetcontrast();
    do
    {
        ozsetcontrast(contr);
        sprintf(buf,"Contrast = %d    ",contr=ozgetcontrast());
        ozputs(0,10,buf);
        switch(ozgetch())
        {
            case KEY_LOWER_ESC:
            case KEY_UPPER_ESC:
            case KEY_MYPROGRAMS:
                return;
            case KEY_UP:
                contr++;
                break;
            case KEY_DOWN:
                contr--;
                break;
        }
    } while(1);
}
