/* compresses a text into wizbook format */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

/* requires a 32-bit compiler */

#define MAX_HEADER 16384
#define MAX_DICTLEN (126+254+254)

unsigned char header[MAX_HEADER];

char *dictionary;

int numwords,indexlen;

char *dict_idx[MAX_DICTLEN];

void err(char *s, ...)
{
    va_list argptr;
    va_start(argptr,s);
    vfprintf(stderr,s,argptr);
    va_end(argptr);
    exit(1);
}

void msg(char *s, ...)
{
    va_list argptr;
    va_start(argptr,s);
    vfprintf(stderr,s,argptr);
    va_end(argptr);
}

char uppercase;
int trimat;

void dumpfromdict(int word)
{
    register char *p=dict_idx[word];
    int shown=0;
    while(!(*p & 0x80) && shown<trimat)
    {
	if(uppercase)
	{
	    putchar(toupper(*p));
	    uppercase=0;
	}
	else putchar(*p);
	shown++;
	p++;
    }
    if(shown<trimat) putchar(*p & 0x7f);
    trimat=254;
}

int getword(FILE *in)
{
    unsigned int n;
    n=fgetc(in);
    n+=256*(unsigned int)fgetc(in);
    return n;
}

int main(int argc, char **argv)
{
    int c;
    int i;
    int start=1;
    int afterword=0;
    int dlen;
    int hlen;
    int notafterword;
    FILE *in;
    register char *p;
    char hdr[8];
    if(argc<2) err("WizDComp InName");
    in=fopen(argv[1],"rb");
    if(in==NULL) err("Error opening %s\n",argv[1]);
    fread(hdr,8,1,in);
    if(strncmp(hdr,"WizBookA",8)) err("Bad header!\n");
    hlen=getword(in);
    if(hlen>MAX_HEADER) err("Header too large!\n");
    getword(in);
    getword(in);
    fread(header,hlen-4,1,in);
    numwords=(unsigned int)header[0]+256*(unsigned int)header[1];
    dlen=(unsigned int)header[2]+256*(unsigned int)header[3];
    dictionary=p=header+4;
    indexlen=dictionary[dlen];
    notafterword=0;
    trimat=254;
    for(i=0;i<numwords;i++)
    {
	dict_idx[i]=p;
	while(!(*p & 0x80)) p++;
	p++;
    }
    while(-1!=(c=fgetc(in)))
    {
/*        printf("<%02X>",c); */
	NEXT:
	if(c<2)
	{
	    if(c==1)
	    {
		int i;
		for(i=0;i<indexlen;i++) fgetc(in); /* skip index data if any */
	    }
	    if(!start)
		putchar('\n');
	    else
		start=0;
	    afterword=0;
	}
	else
	switch(c)
	{
            case 2:
		putchar(',');
                putchar(' ');
                afterword=0;
                break;
	    case 3:
		putchar('.');
                putchar(' ');
		afterword=0;
		break;
            case 127:
                putchar('?');
                putchar(' ');
                afterword=0;
		break;
	    case 129:
		notafterword=1;
		trimat=fgetc(in);
		c=fgetc(in);
		goto NEXT;
	    case 128:
		uppercase=1;
		c=fgetc(in);
                goto NEXT;
            case 30:
                if(afterword) putchar(' ');
                dumpfromdict(fgetc(in)-2+126);
		if(!notafterword)
                {
                    afterword=1;
                }
		else
                {
                    afterword=notafterword=0;
		}
		break;
            case 31:
		if(afterword) putchar(' ');
		dumpfromdict(fgetc(in)-2+126+254);
		if(!notafterword)
		{
                    afterword=1;
                }
                else
                {
		    afterword=notafterword=0;
                }
		break;
	    default:
		if(c>=130)
                {
		    if(afterword) putchar(' ');
		    dumpfromdict(c-130);
		    if(!notafterword)
		       afterword=1;
		    else
		    {
		       afterword=notafterword=0;
		    }
		}
		else
		if(c<32)
                {
                    if(afterword) putchar(' ');
                    putchar(c-4+'a');
                    putchar(' ');
		    afterword=0;
                }
                else
                {
		    if(afterword && isalpha(c)) putchar(' ');
                    putchar(c);
                    afterword=0;
		}
		break;
         }
     }
     return 0;
}
