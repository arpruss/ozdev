
#include	<math.h>

double
tanh(_x)
double  _x;
{
    static double x;
    x = exp(_x);
	return (x-1.0/x)/(x+1.0/x);
}

