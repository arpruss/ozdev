double
eval_poly(x, d, n)
double	x, d[];
int	n;
{
    static int i;
    static double  res;
    static double x0;
    x0=x;

	res = d[i = n];
	while(i)
        res = x0 * res + d[--i];
	return res;
}

