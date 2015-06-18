#include	<math.h>

extern double	eval_poly();
double
exp(_x)
double _x;
{
    static int exp;
    static char    sign;

	static double coeff[] =
	{
		1.0000000000e+00,
		6.9314718056e-01,
		2.4022650695e-01,
		5.5504108945e-02,
		9.6181261779e-03,
		1.3333710529e-03,
		1.5399104432e-04,
		1.5327675257e-05,
		1.2485143336e-06,
		1.3908092221e-07,
	};
    static double x;
    x=_x;

	if(x == 0.0)
		return 1.0;
	sign = x < 0.0;
	if(sign)
		x = -x;
                      /* convert to log2 before other things */
    exp = (int)floor(x *= 1.4426950409);
    if(exp>63)
    {
        __fperr=1;
        return 0.0;
    }
    x = ldexp(eval_poly(x-(double)exp, coeff, sizeof coeff/sizeof coeff[0] - 1), exp);
	if(sign)
		return 1.0/x;
	return x;
}

double
pow(x, y)
double	x, y;
{
	if(y == 0.0)
		return 1.0;
	if(x < 0.0)
    {
        __fperr=1;
		return 0.0;
    }
	if(x == 0.0)
    {
        if(y==0.0) __fperr=1;
		return 0.0;
    }
    return exp(log(x) * y);
}

