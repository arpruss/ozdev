memset(p, c, n)
register char *	p;
register int	n;
char		c;
{
	while(n--)
		*p++ = c;
}

