/*** modified from Hi-Tech C library ***/

/*
 *	Formatted number printing for Z80 printf and debugger
 */
#define	NDIG	30		/* max number of digits to be printed */

#define putch(x)    *output++=(x)

_spnum(i, f, w, s, base, out)
unsigned long	i;
unsigned char	base;
char **out;
char	f, w;
{
	register char *	cp;
    static unsigned char   fw;
    static char *output;
    static char        buf[NDIG];
    output=*out;

	if(f > NDIG)
		f = NDIG;

	if(s && (long)i < 0)
		i = -i;
	else
		s = 0;
	if(f == 0 && i == 0)
		f++;

	cp = &buf[NDIG];
	while(i || f > 0) {
		*--cp = "0123456789ABCDEF"[i%base];
		i /= base;
		f--;
	}
	fw = f = (&buf[NDIG] - cp) + s;
	if(fw < w)
		fw = w;
	while(w-- > f)
		putch(' ');
	if(s) {
		putch('-');
		f--;
	}
	while(f--)
		putch(*cp++);
    *out=output;
	return fw;
}

