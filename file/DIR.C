#include <stdlib.h>
#include <oz.h>

main()
{
    static int pos=0;
    static byte mode=FIND_RESET;
    register char *name;
    ozcls();
    while(NULL!=(name=ozgetnextfilename(mode)))
    {
        if(pos>=72)
        {
            ozputs(0,pos,"Press any key to continue");
            ozgetch();
            ozcls();
            pos=0;
        }
        mode=0;
        ozputs(0,pos,name);
        pos+=8;
    }
    ozputs(0,pos,"Done: press any key to continue");
    ozgetch();
}

