#include <oz.h>

main()
{
    ozputs(10,10,"Saving screen!");
    ozsavescreen();
    ozgetch();
    ozputs(10,10,"Saved the screen!");
    ozgetch();
    ozrestorescreen();
    ozgetch();
}
