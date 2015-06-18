#include	<math.h>

double
acos(x)
double	x;
{
    return atan(sqrt(1-x*x)/x);
}

