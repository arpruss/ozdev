#include	<math.h>

extern double	eval_poly();
double
log(_x)
double  _x;
{
    static int exp;
    static double x;

	static double coeff[] =
	{
		 0.0000000000,	/* a0 */
		 0.9999964239,	/* a1 */
		-0.4998741238,	/* a2 */
		 0.3317990258,	/* a3 */
		-0.2407338084,	/* a4 */
		 0.1676540711,	/* a5 */
		-0.0953293897,	/* a6 */
		 0.0360884937,	/* a7 */
		-0.0064535442,	/* a8 */
	};

	/* zero or -ve arguments are not defined */

    if(_x <= 0.0)
    {
        __fperr=1;
		return 0.0;
    }
    x = frexp(_x, &exp) * 2.0 - 1.0;
	exp--;
	x = eval_poly(x, coeff, sizeof coeff/sizeof coeff[0] - 1);
	return x + 0.69314718055995 * exp;
}

double
log10(x)
double	x;
{
	return log(x) * 0.4342944819;
}

