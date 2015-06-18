#include <oz.h>

main()
{
    ozputs(0,70,"Hello!");
    while(ozgetch()!='q')
        ozscrollclear();
}

