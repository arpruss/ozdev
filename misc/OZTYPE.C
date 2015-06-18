#include <oz.h>

main()
{
    int x=0,y=0;
    int c;
    ozungetch('a');
    ozungetch('b');
    ozungetch('c');
    ozungetch('d');
    ozungetch('e');
    ozungetch('f');
    while((c=ozgetch())!=KEY_LOWER_ESC)
    {
        x=ozputch(x,y,c);
        if(x>230)
        {
            y+=10;
            x=0;
        }
    }
}
