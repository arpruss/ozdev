#include	<ctype.h>

double
atof(char *_s)
{
    static char    sign;
    static double  l;
    static short   exp;
    static short   eexp;
    static char    expsign;
    register char * s=_s;

    while(isspace(*s))
		s++;
	expsign = sign = 0;
	if(*s == '-') {
		s++;
		sign = 1;
	}
	l = 0;
	exp = 0;
	while(isdigit(*s))
        l = 10.*l + (double)(*s++ - '0');
	if(*s == '.')
		while(isdigit(*++s)) {
			exp--;
            l = 10.*l + (double)(*s - '0');
		}
	eexp = 0;
	if(*s == 'e' || *s == 'E') {
		if(*++s == '-') {
			expsign = 1;
			s++;
		}
		if(*s == '+')
			s++;
		while(isdigit(*s))
			eexp = eexp*10 + *s++ - '0';
		if(expsign)
			eexp = -eexp;
	}
	exp += eexp;
	while(exp < 0) {
        l = l / 10.;
		exp++;
	}
	while(exp > 0) {
        l = l * 10.;
		exp--;
	}
	if(sign)
		return -l;
	return l;
}

