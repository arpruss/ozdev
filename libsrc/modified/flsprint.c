#include	<stdio.h>
#include	<ctype.h>

/*
 *  sprintf for 8086 -- modified from doprnt.c by ARP
 */

static uchar	ival;
static char *	x;
extern int	atoi(char *);
extern int	strlen(char *);

#define pputc(c) *output++=(c)

static char *
icvt(cp)
register char *	cp;
{
	ival = atoi(cp);
	while(isdigit((unsigned)*cp))
		cp++;
	return cp;
}

sprintf(s, f, arg1)
char *s;
register char *     f;
int arg1;
{
    register char *output=s;
    static int *a;
    static char    c, prec;
    static char *o1;
    static uchar   fill, left;
    static uchar   i;
    static uchar   base, width, sign, len;
    static uchar   ftype;
    extern  short _spnum(), _sfnum();

    a=&arg1;

	while(c = *f++)
		if(c != '%')
			pputc(c);
		else {
			base = 10;
			width = 0;
			sign = 0;
			left = 0;
			ftype = 0;
			len = sizeof(int)/sizeof *a;
			if(*f == '-') {
				f++;
				left++;
			}
			fill = *f == '0';
			if(isdigit((unsigned)*f)) {
				f = icvt(f);
				width = ival;
			} else if(*f == '*') {
				width = *a++;
				f++;
			}
			if(*f == '.')
				if(*++f == '*') {
					prec = *a++;
					f++;
				} else {
					f = icvt(f);
					prec = ival;
				}
			else
				prec = fill ? width : -1;
			if(*f == 'l') {
				f++;
				len = sizeof(long)/sizeof *a;
			}
			switch(c = *f++) {

			case 0:
                *output=0;
				return;
			case 'o':
			case 'O':
				base = 8;
				break;
			case 'd':
			case 'D':
				sign = 1;
				break;

			case 'x':
			case 'X':
				base = 16;
				break;

			case 's':
				x = *(char **)a;
				a += sizeof(char *)/sizeof *a;
				if(!x)
					x = "(null)";
				i = strlen(x);
dostring:
				if(prec < 0)
					prec = 0;
				if(prec && prec < i)
					i = prec;
				if(width > i)
					width -= i;
				else
					width = 0;
				if(!left)
					while(width--)
						pputc(' ');
				while(i--)
					pputc(*x++);
				if(left)
					while(width--)
						pputc(' ');
				continue;
			case 'c':
				c = *a++;
			default:
				x = &c;
				i = 1;
				goto dostring;

			case 'u':
			case 'U':
				break;

			case 'e':
			case 'E':
				sign++;

			case 'g':
			case 'G':
				sign++;

			case 'f':
			case 'F':
				if(prec < 0)
					prec = 6;
				ftype = 1;
				break;
			}
			if(left) {
				left = width;
				width = 0;
			}
			if(isupper(c))
				len = sizeof(long)/sizeof *a;
			if(prec < 0)
				prec = 0;
			if(ftype) {
                o1=output;
                width = _sfnum(*(double *)a, prec, width, sign, &o1);
                output=o1;
				a += sizeof(double)/sizeof(*a);
			} else {
                o1=output;
                width = _spnum((len == sizeof(int)/sizeof *a ? (sign ?
                (long)*a : (unsigned long)*a) : *(long *)a), prec,
                width, sign, base, &o1);
                output=o1;
				a += len;
			}
			while(left-- > width)
				pputc(' ');
		}
    *output=0;
}

