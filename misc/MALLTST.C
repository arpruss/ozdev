#include <stdlib.h>

close(){;}

main()
{
    char s1[80];
    char *s;
    ozputs(0,0,"Starting...");
    s=sbrk(2048);
    sprintf(s1,"sbrk(2048)=%x\n",s);
    ozputs(0,0,s1);
    s=malloc(2048);
    sprintf(s1,"malloc(2048)=%x\n",s);
    ozputs(0,10,s1);
    s=malloc(1024);
    sprintf(s1,"malloc(1024)=%x\n",s);
    ozputs(0,20,s1);
    s=malloc(14000);
    sprintf(s1,"malloc(14000)=%x\n",s);
    ozputs(0,30,s1);
}
