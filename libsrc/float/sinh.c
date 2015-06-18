#include	<math.h>

double
sinh(_x)
double  _x;
{
    static double x;
    x = exp(_x);
	return 0.5*(x-1.0/x);
}

