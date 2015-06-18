#include <oz.h>

/* sprite */

main()
{
	int i;
	for(i=0;i<200;i++) putchar(i+'A');
	_ozputsprite(10,10,4,"\x0F\xF0\x09\xF6\x09\xF6\x0F\xF0");
	ozgetch();
}
