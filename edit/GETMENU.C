#include <oz.h>

main()
{
    ozcls();
    ozputs(0,0,"Choose one:");
    ozputs(0,10,"(L)oad file from list");
    ozputs(0,20,"Load file by (n)ame");
    ozputs(0,30,"(C)reate file");
    ozputs(0,40,"(D)elete one file or delete (a)ll owned files");
    ozputs(0,50,"(Any other key to exit.)");
    ozputs(0,70,"In editor, press MENU for help.");
    ozsnap();
    ozsnap();
}

