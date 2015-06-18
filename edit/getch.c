#include <oz.h>
#include <ctype.h>

#define HIBYTE(x) (*(1+(byte *)&(x)))

static unsigned ozkeyread(byte read)
{
	static byte isshifted=0;
	static byte is2nded=0;
	static byte iscapslock=0;
	static byte isctrl=0;
	static byte iscat=0;
	static byte shiftheld=0;
	static byte catheld=0;
	static byte secondheld=0;
	static byte ctrlheld=0;
    static union
    {
        unsigned int w;
        struct
        {
            byte l,h;
        } b;
    } c_word,origc_word;
#define origc (origc_word.w)
#define c (c_word.w)
/*	  termputch(isshifted?'+':'-'); */
	if(!read && !ozkeyhit()) return NO_KEY;
    origc=ozgetchblank();
    c=origc & 0xF0FF;
	switch(c)
	{
		case KEY_CATEGORY:
           iscat^=1; /* !iscat; */
		   return NO_KEY;
		case KEY_LEFT_SHIFT & 0xF0FF:
		   if(is2nded)
		   {
              iscapslock^=1;
			  iscat=is2nded=0;
			  return NO_KEY;
		   }
		case KEY_RIGHT_SHIFT:
/*			 termputch('!'); */
           isshifted^=1;
		   return NO_KEY;
		case KEY_2ND:
           is2nded^=1;
		   return NO_KEY;
		case KEY_NEW:
           isctrl^=1;
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
           if(c=='\r')
           {
              c='\n';
           }
		default:
		   if(!read) break;
           if(isshifted || (origc_word.b.h&(MASKSHIFT>>8))) shiftheld=1; else shiftheld=0;
           if(isctrl || (origc_word.b.h&(MASKCTRL>>8))) ctrlheld=1; else ctrlheld=0;
           if(iscat || (origc_word.b.h&(MASKCATEGORY>>8))) catheld=1; else catheld=0;
           if(is2nded || (origc_word.b.h&(MASK2ND>>8))) secondheld=1; else secondheld=0;
/*           c_word.b.h &=0xF0; */
           if(catheld && !ctrlheld)
		   {
			   switch(c)
			   {
				 case 'o': c='['; break;
				 case 'p': c=']'; break;
				 case '9': c='{'; break;
				 case '0': c='}'; break;
				 case 'r': c='\\'; break;
				 case 'w': c='`'; break;
                 case 'l':
				 case 'q': c='~'; break;
				 case '1': c='|'; break;
				 case ',': c='<'; break;
				 case '.': c='>'; break;
                 default: c_word.b.h|=MASKCATEGORY>>8; break;
				}
		   }
           else if(ctrlheld && !catheld && c<128 && isalpha(c))
			   c-='a'-1;
		   else if(ctrlheld && c==' ')
			   c=0;
		   else if(ctrlheld && catheld)
		   {
			   switch(c)
			   {
				 case 'o': c=27; break;
				 case 'p': c=29; break;
				 case 'r': c=28; break;
				 case 'q': c=30; break;
			   }
		   }
		   else if(ctrlheld && secondheld && c=='i')
				  c=31;
		   else if(ctrlheld && c=='6')
				  c=0x1E;
		   else if(ctrlheld && c=='-')
				  c=0x1F;
           else if((shiftheld || iscapslock) && c<128 && isalpha(c) && !ctrlheld
			   && !catheld && !secondheld)
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
                default: c_word.b.h|=MASK2ND>>8; break;
			  }
              if(ctrlheld) c_word.b.h |= MASKCTRL >> 8; /* c|=MASKCTRL; */
              if(shiftheld) c_word.b.h |= (MASKSHIFT>>8);
		   }
           else if(ctrlheld || shiftheld)
           {
               if(ctrlheld) c_word.b.h|=MASKCTRL>>8;
               if(shiftheld) c_word.b.h|=MASKSHIFT>>8;
           }
	 }
	if(!read)
		ozungetch(origc);
	else
		isshifted=is2nded=isctrl=iscat=0;
	return c;
}
#undef c
#undef origc

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
