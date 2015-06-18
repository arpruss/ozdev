#include <oz.h>

main()
{
    int h,h2;
    char s[44];
    h=open("test1",O_WRONLY);
    write(h,"hello",5);
    close(h);
    h=open("test1",O_RDONLY);
    h2=open("test2",O_WRONLY);
    sprintf(s,"%d %d",h,h2);
    ozputs(10,10,s);
    ozgetch();
}


