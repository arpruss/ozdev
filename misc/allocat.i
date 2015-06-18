# 1 "allocat.c"

# 1 "c:\progra~1\wiz\hitech\compiler/alloca.h"
void *__x_alloca_19828a_(unsigned length);
# 3 "allocat.c"
int i;

z()
{
static char *s;
s=__x_alloca_19828a_((4));
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
