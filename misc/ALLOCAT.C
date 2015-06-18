#include <alloca.h>

int i;

z()
{
    static char *s;
    s=alloca(3);
    strcpy(s,"He");
    ozputs(0,i,"He");
    stack();
}

stack()
{
    int i;
    printf("[%x]",&i);
}

main()
{
    int k=0;
    i=k; k+=10;
    z();
    stack();
    i=k; k+=10;
    z();
    stack();
    i=k; k+=10;
    z();
    stack();
    i=k; k+=10;
    z();
    stack();
    ozgetch();
}
