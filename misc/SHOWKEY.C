#include <oz.h>

main()
{
    char buf[8];
    sprintf(buf,"%x",getch());
    ozputs(10,10,buf);
    ozgetch();
}
