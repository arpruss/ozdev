/*** modified from Hi-Tech C library ***/
#include    <stdio.h>
#include	<ctype.h>
#include	<string.h>
#include    "oz.h"


/*
 *	doprnt for Z80
 */

extern int	atoi(char *);
extern int  _spnum();

static uchar	ival;

#define pputc(c) ( putchar(c), length++ )

static char *
icvt(cp)
register char *	cp;
{
	ival = atoi(cp);
	while(isdigit((unsigned)*cp))
		cp++;
	return cp;
}

printf(f, arg1)
register char *     f;
int arg1;
{
    register char *x;
    static int *a;
    static char    c;
    static uchar   fill, left;
    static uchar   i;
    static uchar   base, width, prec, sign, len;
    static int length;

    length=0;

    a=&arg1;

	while(c = *f++)
		if(c != '%')
            {
                pputc(c);
            }
		else {
			base = 10;
			width = 0;
			sign = 0;
			left = 0;
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
				prec = fill ? width : 0;
			if(*f == 'l') {
				f++;
				len = sizeof(long)/sizeof *a;
			}
			switch(c = *f++) {

			case 0:
                return length;
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
				break;
			}
			if(left) {
				left = width;
				width = 0;
			}
			if(isupper(c))
				len = sizeof(long)/sizeof *a;
            width = _spnum_p((len == sizeof(int)/sizeof *a ? (sign ?
              (long)*a : (unsigned long)*a) : *(long *)a), prec, width,
              sign, base,&length);
			a += len;
			while(left-- > width)
				pputc(' ');
		}
        return length;
}

