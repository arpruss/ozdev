#include <math.h>

/* uses isin.as */
/* medium fast, interpolated routines */
/* This is faster than the Hi-Tech C trig functions
   and for most values it is more accurate. However,
   it is also bigger */

#define M_2_PI 6.2831853
#define M_PI_2 1.5707963
#define M_RECIP_16384 0.000061035156

int isin(unsigned degrees);
int icos(unsigned degrees);

double fpsin(double x)
{
    static double z,t,flz;
    static unsigned n;
    z=x/M_2_PI;
    z=360.*(z-floor(z));
    t=z-(flz=floor(z));
    n=(unsigned int)flz;
    return M_RECIP_16384*((1-t)*isin(n)+t*isin(n+1));
}

double fpcos(double x)
{
    return fpsin(M_PI_2-x);
}

double fptan(double x)
{
    return fpsin(x)/fpsin(M_PI_2-x);
}
