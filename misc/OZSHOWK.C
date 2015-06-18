#include <oz.h>

close(){}

main()
{
    unsigned c;
    char s[40];
    while(KEY_LOWER_ESC!=(c=ozgetch()))
    {
        sprintf(s,"%x        ",c);
        ozputs(0,10,s);
    }
}
