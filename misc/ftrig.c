#include <math.h>

/* uses isin.as */
/* very fast, one-degree precision routines */

#define M_2_PI 6.2831853
#define M_PI_2 1.5707963
#define M_RECIP_16384 0.000061035156

int isin(unsigned degrees);
int icos(unsigned degrees);

double fsin(double x)
{
    static double z;
    z=x/M_2_PI;
    return M_RECIP_16384*isin((unsigned int)(360*(z-floor(z))));
}


double fcos(double x)
{
    return fsin(M_PI_2-x);
}

double ftan(double x)
{
    static double z;
    static unsigned z1;
    z=x/M_2_PI;
    z1=(unsigned int)(360*(z-floor(z)));
    return (double)isin(z1)/(double)icos(z1);
}

