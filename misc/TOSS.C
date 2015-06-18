#include <oz.h>
#include <stdlib.h>

main()
{
    static byte sides;
    srand(oztime());
    ozfont(FONT_OZ_LARGE);
    ozputs(0,10,"Sides? (1-9)");
    sides=ozgetch()-'0';
    ozputch(115,30,'0'+(rand()%sides));
    ozgetch();
}
