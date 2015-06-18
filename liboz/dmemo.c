#include <oz.h>
#include <string.h>

#define HEADER 43

static unsigned recnum;
static unsigned memo_pos=0;

#define DONOTEDIT_LEN 13

static byte memo[HEADER+MAX_MEMOLEN+1]=
"MEMO\xFF\xFF\xD0\x0D\x00\x16\x00\x00\x00"
"20991230\x00zz:same!.data       \x00"
"DO NOT EDIT!\r";

unsigned ozgetdatamemorecnum(void)
{
    return recnum;
}

int ozreaddatamemo(void *_s,unsigned length)
{
	static int c1,c2;
    static unsigned out;
    static unsigned l;
    register byte *s=_s;
    l=length;
	out=0;
	if(!recnum) return -1;
    while(l)
	{
        if(-1==(c1=ozreadfilebyte()) || !c1)
        {
            recnum=0;
            return out;
        }
		if(c1<127-16)
		{
            *s++=(byte)c1-(byte)33;
		}
		else
		{
            if(-1==(c2=ozreadfilebyte()) || !c2)
            {
                recnum=0;
                return out;
            }
            *s++ = (byte)(((byte)c1-(byte)(127-16))<<4) | ((byte)c2-(byte)(127-16));
		}
		out++;
        l--;
	}
	return out;
}

int ozopendatamemoread(char *title)
{
    static unsigned a,b,loc;
    static byte i;
	a=b=recnum=0;
	if(!(loc=ozfindmemo(title,&a,&b,&recnum)))
	{
		recnum=0;
		return -1;
	}
	ozopenfile(loc);
    for(i=HEADER+DONOTEDIT_LEN;i;i--) ozreadfilebyte();
	return ozreadfilebyte();
}

void ozclosedatamemoread(void)
{
	recnum=0;
	ozclosefile();
}

int ozwritedatamemo(void *_s,unsigned length)
{
    byte *s=_s;
    static byte datum;
    if(memo_pos>=MAX_MEMOLEN+HEADER) return -1;
    while(length--)
	{
        datum=*s;
        if(datum<127-16-33)
		{
			if(memo_pos<MAX_MEMOLEN+HEADER)
                memo[memo_pos++]=33+datum;
			else return length+1;
		}
		else
		{
            if(memo_pos<MAX_MEMOLEN+HEADER-1)
			{
                memo[memo_pos]=(byte)(datum>>4) + (byte)(127-16);
                (memo+1)[memo_pos]=(byte)(datum&(byte)0xF) + (byte)(127-16);
				memo_pos+=2;
			}
			else return length+1;
		}
        s++;
	}
}

void ozopendatamemowrite(char *title,char version)
{
	static unsigned a,b,loc,recnum;
    static char innertitle[21];
    static byte i;
    memcpy(innertitle,title,20);
    innertitle[20]=0;
    for(i=strlen(innertitle);i<20;i++) innertitle[i]=' ';
    innertitle[20]=0;
	a=b=recnum=0;
    if((loc=ozfindmemo(innertitle,&a,&b,&recnum)))
		ozunlink(TYPENUM_MEMO,recnum);
    memcpy(memo+HEADER-21,innertitle,21);
    memo[HEADER+DONOTEDIT_LEN]=version;
    memo_pos=HEADER+DONOTEDIT_LEN+1;
}

int ozclosedatamemowrite(void)
{
	memo[memo_pos++]=0;
	return ozwritefile(memo,memo_pos,TYPENUM_MEMO);
}


