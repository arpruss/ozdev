#include <oz.h>
#include <ctype.h>

static byte mapreturn=1;
static byte enterisreturn=1;

void ozenterisreturn(byte map)
{
    enterisreturn=map;
}

void ozkeymapcrtolf(byte map)
{
	mapreturn=map;
}

static unsigned ozkeyread(byte read)
{
	static byte isshifted=0;
	static byte is2nded=0;
	static byte iscapslock=0;
	static byte shiftheld=0;
	static byte secondheld=0;
	static unsigned int c;
	static unsigned int origc;
/*	  termputch(isshifted?'+':'-'); */
	if(!read && !ozkeyhit()) return NO_KEY;
	origc=ozgetch();
	c=origc&0xF0FF;
	switch(c)
	{
		case KEY_LEFT_SHIFT & 0xF0FF:
		   if(is2nded)
		   {
              iscapslock^=1;
              is2nded=0;
			  return NO_KEY;
		   }
		case KEY_RIGHT_SHIFT:
/*			 termputch('!'); */
           isshifted^=1;
		   return NO_KEY;
		case KEY_2ND:
           is2nded^=1;
		   return NO_KEY;
		case KEY_DEL:
		   c=8;
		   break;
		case KEY_LOWER_ESC:
		   c=27;
		   break;
		case KEY_LOWER_ENTER:
		case KEY_UPPER_ENTER:
		case '\r':
           if(c=='\r' || enterisreturn)
           {
              if(mapreturn) c='\n'; else c='\r';
           }
		default:
		   if(!read) break;
		   if(isshifted || origc&MASKSHIFT) shiftheld=1; else shiftheld=0;
		   if(is2nded || origc&MASK2ND) secondheld=1; else secondheld=0;
		   c&=0xF0FF;
           if((shiftheld || iscapslock) && c<128 && isalpha(c)
               && !secondheld)
               {
                    if(shiftheld ^ iscapslock) c+='A'-'a';
               }
           else if((shiftheld /* || secondheld */) && c=='-')
			   c='_';
           else if(shiftheld && c<128 && isdigit(c))
			   c=")!@#$%^&*("[c-'0'];
           else if((/* secondheld || */ shiftheld) && c=='.')
			   c=':';
           else if((/* secondheld || */ shiftheld) && c==',')
               c='\'';
		   else if(secondheld)
		   {
			  switch(c)
			  {
				case 'e': c='*'; break;
				case 'i': c='?'; break;
				case 'o': c='"'; break;
				case 'p': c=';'; break;
				case 'q': c='+'; break;
				case 'r': c='/'; break;
				case 't': c='='; break;
				case 'w': c='-'; break;
                default: c|=MASK2ND; break;
			  }
              if(shiftheld) c|=MASKSHIFT;
		   }
           else if(shiftheld)
           {
               c|=MASKSHIFT;
           }
	 }
	if(!read)
		ozungetch(origc);
	else
        isshifted=is2nded=0;
	return c;
}

int kbhit(void)
{
	static int k;
	static int l;
	while((k=ozkeyhit()) && (l=(NO_KEY==ozkeyread(0))));
	return k && !l;
}

unsigned getch(void)
{
	static unsigned c;
    while(NO_KEY==(c=ozkeyread(1))) ozintwait();
	return c;
}

/*
main()
{
	int x,y,c;
	x=0;
	y=0;
	while((c=getch())!=27)
	{
		x=ozputch(x,y,c);
		if(x>230)
		{
			y+=10;
			x=0;
		}
	}
}
*/
