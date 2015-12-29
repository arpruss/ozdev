#include <oz.h>
#include <ctype.h>
#define ozgetch _ozgetch

#define NO_KEY		 0xFFFF

#define MASKSHIFT	 0x0800
#define MASKCTRL	 0x0200
#define MASKCATEGORY 0x0400
#define MASK2ND 	 0x0100

unsigned mygetch(void)
{
	static byte isshifted=0;
	static byte is2nded=0;
	static byte iscapslock=0;
	static byte iscat=0;
	static byte shiftheld=0;
	static byte catheld=0;
	static byte secondheld=0;
	static unsigned int c;
	static unsigned int origc;
/*	  termputch(isshifted?'+':'-'); */
	origc=ozgetch();
	c=origc&0xF0FF;
	switch(c)
	{
        case KEY_POWER:
        case KEY_LOWER_ESC:
        case KEY_UPPER_ESC:
        case KEY_MYPROGRAMS:
            exit(0);
        case KEY_CATEGORY:
		   iscat=!iscat;
		   return NO_KEY;
		case KEY_LEFT_SHIFT & 0xF0FF:
		   if(is2nded)
		   {
			  iscapslock=!iscapslock;
			  iscat=is2nded=0;
			  return NO_KEY;
		   }
		case KEY_RIGHT_SHIFT:
		   isshifted=!isshifted;
		   return NO_KEY;
		case KEY_2ND:
		   is2nded=!is2nded;
		   return NO_KEY;
		case KEY_DEL:
		   c=8;
		   break;
		case KEY_LOWER_ENTER:
		case KEY_UPPER_ENTER:
		case '\r':
           c='\n';
		   break;
		default:
		   if(isshifted || origc&MASKSHIFT) shiftheld=1; else shiftheld=0;
		   if(iscat || origc&MASKCATEGORY) catheld=1; else catheld=0;
		   if(is2nded || origc&MASK2ND) secondheld=1; else secondheld=0;
		   c&=0xF0FF;
           if(catheld)
		   {
			   switch(c)
			   {
				 case 'o': c='['; break;
				 case 'p': c=']'; break;
				 case '9': c='{'; break;
				 case '0': c='}'; break;
				 case 'r': c='\\'; break;
				 case 'w': c='`'; break;
				 case 'q': c='~'; break;
				 case '1': c='|'; break;
				 case ',': c='<'; break;
				 case '.': c='>'; break;
				 default: c|=MASKCATEGORY; break;
				}
		   }
           else if((shiftheld ^ iscapslock) && isalpha(c)
			   && !catheld && !secondheld)
               c+='A'-'a';
		   else if((shiftheld || secondheld) && c=='-')
			   c='_';
		   else if(shiftheld && isdigit(c))
			   c=")!@#$%^&*("[c-'0'];
		   else if((secondheld || shiftheld) && c=='.')
			   c=':';
		   else if((secondheld || shiftheld) && c==',')
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
		   }
	 }
    isshifted=is2nded=iscat=0;
	return c;
}

