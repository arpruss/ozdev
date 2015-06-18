#include	<math.h>

extern double	_frndint();

double
floor(x)
double	x;
{
    static double  i;

	i = _frndint(x);
	if(i > x)
		return i - 1.0;
	return i;
}

