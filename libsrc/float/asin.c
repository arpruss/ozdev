#include	<math.h>

double
asin(x)
double	x;
{
    return atan(x/sqrt(1-x*x));
}

