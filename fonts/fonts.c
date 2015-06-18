#include <string.h>
#include <oz.h>
#include "fonts.h"

typedef char fname[MAX_FILENAMELEN+1];

char anykey[]="Press any key";
char yn[]="Yes:[ENTER]/No:[ESC]";
char directory[]="fn/";
char fontline1[]="AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
char fontline2[]="0123456789?!@#$%^&*()-_=+[]{}\\|;:'\",.<>/?`~";
char fontline3[]="The quick brown fox jumped over the lazy dog.";

byte font[8192];

char *ozgetnextfilename0(byte mode)
{
	register char *s;
	while(NULL!=(s=ozgetnextfilename(mode)))
	{
		if(strncmp(s,"bm/",3)) return s;
		mode &= ~FIND_RESET;
	}
	return NULL;
}

byte nof=0;

unsigned mygetch(void)
{
    static unsigned k;
    while(1)
    {
        k=ozgetchblank();
        switch(k)
        {
             case KEY_MAIN:
             case KEY_TELEPHONE:
             case KEY_SCHEDULE:
             case KEY_MEMO:
             case KEY_MYPROGRAMS:
                ozexitto(k);
             case KEY_BACKLIGHT:
                oztogglelight();
                break;
             case 'z':
                ozsnap();
                ozsnap();
                break;
             default: return k;
        }
    }
}


void process(char *f)
{
    static byte h;
    static unsigned length;
    static byte load;
    ozcls();
    ozfont(FONT_OZ_NORMAL);
    ozputs(0,0,f);
    if(open1(f,O_RDONLY)<0)
    {
        ozwarn("Cannot open!",anykey);
        return;
    }
    read1(font,8192);
    close1();
    if(length > 2*MAX_CUSTOM_SLOT_SIZE)
    {
        ozwarn("Font file too big!",anykey);
        return;
    }
    ozsetfontpointer(font);
    h=ozgetfontheight(ozgetfont());
    ozputs(0,10,fontline1);
    ozputs(0,10+h,fontline2);
    ozputs(0,10+h*2,fontline3);
    ozfont(FONT_OZ_NORMAL);
    if(length> MAX_CUSTOM_SLOT_SIZE)
        ozputs(0,72,"0: Load into slot 0.  ESC: Cancel.");
    else
        ozputs(0,72,"0/1: Load into slot 0/1.  ESC: Cancel.");
    load=0xFF;
    switch(mygetch())
    {
        case '0':
            load=0;
            break;
        case '1':
            if(length<=MAX_CUSTOM_SLOT_SIZE) load=1;
            break;
    }
    if(load!=0xFF)
         ozwritecustomfontmem(load,font);
}

#define SLOT_HEIGHT 36

void viewslot(byte slot,byte y,byte status,byte font)
{
    static byte x;
    static byte h;
    static byte pos;
    ozsetfont(FONT_OZ_NORMAL);
    x=putformatted(0,y,"Slot |: ",(unsigned int)slot);
    if(status)
    {
        ozsetfont(font);
        ozputs(x,y,fontline1);
        pos=0;
        h=ozgetfontheight(font);
        if(h<8) pos+=8; else pos+=h;
        if(pos+h<=SLOT_HEIGHT)
        {
            ozputs(0,y+pos,fontline2);
            pos+=h;
            if(pos+h<=SLOT_HEIGHT)
                ozputs(0,y+pos,fontline3);
        }
        ozsetfont(FONT_OZ_NORMAL);
    }
    else
        ozputs(x,y,"(empty)");
}

void viewslots(void)
{
    static byte status;
    static unsigned k;
    do
    {
        ozcls();
        status=ozscancustomfonts();
        viewslot(0,0,status&1,FONT_CUSTOM0);
        viewslot(1,SLOT_HEIGHT,status&2,FONT_CUSTOM1);
        ozputs(0,72,"0/1: clear slot 0/1, anything else: exit");
        k=mygetch();
        if(k=='0' || k=='1')
            ozclearcustomfontslot(k-'0');
        else
            return;
    } while(1);
}

void nofiles(void)
    {
        ozcls();
        if(ozwarn("No files!  View slots?",yn)==KEY_LOWER_ENTER)
        {
            viewslots();
        }
        exit(0);
    }



void selectname(void)
{
	static fname files[9];
	static byte refresh;
	static byte pos;
	static unsigned i;
	static unsigned i1;
	static unsigned k;
	static unsigned curpos;
	static unsigned cursorloc;
	register char *f;
	cursorloc=curpos=0;
	ozfont(FONT_OZ_NORMAL);
	f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
	if(f==NULL) nofiles();
	do
	{
        ozcls();
		for(pos=i=0;i<9;i++,pos+=8)
		{
			if(i && NULL==(f=ozgetnextfilename0(FIND_OWNED))) break;
			strcpy(files[i],f);
			putformatted(0,pos,"|. $",i+1,f);
/*
			ozputch(0,pos,i+'1');
			ozputch(5,pos,'.');
			ozputs(9,pos,f);
*/
		}
		curpos+=9;
		if(cursorloc>=i) cursorloc=i-1;
        ozputs(0,72,"\x0A/\x0B/\x0D/1-9:choose  Del:del.  A:del. all  S:slots  ESC:exit");
		if(NULL!=f) f=ozgetnextfilename0(FIND_OWNED);
		refresh=0;
		do
		{
		  _ozfilledbox(0,8*cursorloc,150,8,XOR);
          k=mygetch();
		  _ozfilledbox(0,8*cursorloc,150,8,XOR);
		  switch(k)
		  {
             case KEY_LOWER_ESC:
             case KEY_UPPER_ESC:
                exit(0);
             case 's':
               ozsavescreen();
               viewslots();
               ozrestorescreen();
               break;
			 case 'a':
               ozsavescreen();
			   if(ozwarn("Are you sure you want to delete all?",yn)!=
				KEY_LOWER_ENTER)
			   {
                    ozrestorescreen();
					break;
			   }
			   ozunlinkallowned();
			   exit(0);
			 case KEY_BACKSPACE:
               ozsavescreen();
			   ozcls();
			   f=files[cursorloc];
               ozputs(ozputs(0,0,"Deleting: "),0,f);
			   if(ozwarn("Are you sure?",yn)==KEY_LOWER_ENTER)
			   {
				   if(k==KEY_BACKSPACE) unlink(f);
                   refresh=1;
                   f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
                   if(f==NULL) nofiles();
                   cursorloc=curpos=0;
               }
               else ozrestorescreen();
			   break;
             default:
               if(k<'1' || k>='1'+i) break;
               cursorloc=k-'1';
             case KEY_LOWER_ENTER:
			 case KEY_UPPER_ENTER:
			 case KEY_RETURN:
                ozsavescreen();
                process(files[cursorloc]);
                ozrestorescreen();
                break;
			 case KEY_UP:
				if(cursorloc) cursorloc--;
				else if(curpos>=18)
				{
					cursorloc=8;
					k=KEY_PAGEUP;
				}
				break;
			 case KEY_DOWN:
				if(cursorloc<i-1) cursorloc++;
				else if(f!=NULL)
				{
					cursorloc=0;
					k=KEY_PAGEDOWN;
				}
				break;
		  }
		  switch(k)
		  {
			case KEY_PAGEUP:
					  if(curpos>=18)
					  {
						curpos-=18;
						f=ozgetnextfilename0(FIND_OWNED | FIND_RESET);
						for(i1=0;i1<curpos;i1++)
						  ozgetnextfilename0(FIND_OWNED);
						refresh=1;
					  }
					  else
						cursorloc=0;
					  break;
			case KEY_PAGEDOWN:
					  refresh=1;
					  if(f==NULL)
					  {
						 cursorloc=i-1;
						 refresh=0;
					  }
					  break;
		  }
		} while(!refresh);
    } while(1);
}

main()
{
    ozcls();
    ozputs(0,0,"Looking for new fonts...");
    frommemo();
    ozsetowner(999);
    selectname();
}

