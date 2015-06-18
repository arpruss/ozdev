#include <oz.h>
#include <ctype.h>
#include <string.h>

#include "ozread.h"
#include "scaldate.h"
#include "viewweek.h"

/* #define DEBUG */

extern byte reverse,underline;
extern byte curcol,currow;

enum repeat_types { DAILY, WEEKDAILY, WEEKLY, TWOWEEKLY, MONTHLY_DATE_BEG,
	MONTHLY_DATE_END,
	MONTHLY_WEEKDAY_BEG,
	MONTHLY_WEEKDAY_END,
	YEARLY, ONLY_TODAY, UNSPECIFIED };

void reverselines(byte y,byte n);

char *itoa(int i);

extern byte propfont;
extern byte mainheader;
extern long startdate;
extern char alignat;
extern byte viewempty;
extern char viewheaders;
extern byte view_todos;
extern byte view_annivs;
extern byte view_appts;
extern byte overflowed;
extern byte repeatchooser;
extern byte silent;
extern int found;

typedef struct
{
	unsigned loc;
	char type;
} appts_t;

extern appts_t appts[NUMDAYS][MAXAPPTS];
extern int num_appts[NUMDAYS];
extern unsigned theday[NUMDAYS];
extern unsigned themonth[NUMDAYS];
extern unsigned theyear[NUMDAYS];

extern char *months[];
extern char *days[];

char entry_space[1026];
unsigned entry_len;

unsigned mygetch()
{
	unsigned c;
	while((c=ozgetch())==KEY_LEFT_SHIFT || c==KEY_RIGHT_SHIFT);
	if(c==KEY_MYPROGRAMS || c==KEY_CALENDAR || c==KEY_MEMO
	   || c==KEY_TELEPHONE) exitto(c);
	return c;
}

byte entrytype;

byte monthlengths[]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

byte x,y;

int getnumber(void)
{
	unsigned k;
	int n=0;
#define MAXDIGITS 5
	static int positions[MAXDIGITS];
	int digits=0;
	positions[0]=x;
	while(KEY_LOWER_ENTER!=(k=mygetch()) && k!=KEY_UPPER_ENTER
		&& k!=13)
		switch(k)
		{
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
				 return -1;
			case 8: /* backspace */
				 if(digits==0) break;
				 if(!propfont)
					termputs("\8 \8");
				 else
					ozputs(x=positions[digits-1],y,"  ");
				 digits--;
				 n/=10;
			default:
				 if(isdigit(k) && digits<MAXDIGITS)
				 {
					n=n*10+k-'0';
					digits++;
					if(propfont) x=positions[digits]=ozputch(x,y,k);
					 else termputch(k);
				 }
		}
	return n;
}


void grabentry(unsigned loc)
{
	unsigned l;
	int c;
	int f;
	register char *p=entry_space;
	int textat;
	switch(entrytype)
	{
		case TYPE_SCHEDULE:
			 textat=36;
			 break;
		case TYPE_TODO:
			 textat=28;
			 break;
		case TYPE_ANNIVERSARY:
			 textat=18;
			 break;
	}
	f=ozopenreadloc(loc);
	for(l=0; l<1023; l++)
		if(-1!=(c=ozgetc(f)) && (c || l<textat))
			*p++=c;
		else break;
	*p++=0;
	entry_len=p-entry_space;
	entry_space[4]=entry_space[5]=0xff;
	ozclose(f);
}

char *utoa_0n(unsigned u,int ndigits)
{
	static char buffer[11];
	register char *b;
	if(ndigits>10) return NULL;
	b=buffer+10;
	*b=0;
	while(ndigits--)
	{
		b--;
		*b=u%10+'0';
		u/=10;
	}
	return b;
}

void putstring(char *s)
{
	if(!propfont)
		termputs(s);
	else
		while(*s)
		{
			if(*s=='\r') x=0;
			 else if(*s=='\n') y+=PROP_FONT_HEIGHT;
			  else x=ozputch(x,y,*s);
			s++;
		}
}

void dupentry(long date)
{
	unsigned y,m,d;
	char *s;
	int date_base;
	byte writeentrytype;
	switch(entrytype)
	{
		case TYPE_SCHEDULE:
			writeentrytype=3;
			date_base=13;
			break;
		case TYPE_TODO:
			writeentrytype=5;
			date_base=15;
			break;
		case TYPE_ANNIVERSARY:
			writeentrytype=4;
			date_base=13-4; /* skip year */
			break;
	}
	scalar_to_ymd(date,&y,&m,&d);
	strncpy(date_base+4+entry_space,s=utoa_0n(m,2),2);
	putstring(s);
	putstring("/");
	strncpy(date_base+6+entry_space,s=utoa_0n(d,2),2);
	putstring(s);
	if(entrytype!=TYPE_ANNIVERSARY)
	{
		putstring("/");
		strncpy(date_base+entry_space,s=utoa_0n(y,4),4);
		putstring(s);
	}
	putstring("\r");
#ifdef DEBUG
{
	int i;
	for(i=0;i<entry_len;i++)
	{
		termputch(entry_space[i]);
		if(!(i%55)) termputs("\r\n");
	}
	mygetch();
}
#endif
	ozwritefile(entry_space,entry_len,writeentrytype);
}

byte monthlength(byte month, unsigned year)
{
	if(month==2 && isleap(year)) return 29;
	  else return monthlengths[month-1];
}

int existday(unsigned y, unsigned m, unsigned d)
{
	if(d<1 || m<1 || m>12 || d>monthlength(m,y)) return 0;
	 else return 1;
}

int killappt(int n)
{
	int appt,index,i;
	unsigned page=0,offset=0,recnum,loc;
	byte typenum;
	for(i=0;i<NUMDAYS;i++)
	{
		if(n<num_appts[i])
		{
			appt=n;
			index=i;
			break;
		}
		n-=num_appts[i];
	}
	if(i==NUMDAYS)
	{
		beep();
		beep();
		beep();
		return 0;
	}
	do
	{
		loc=ozfindnext(appts[index][n].type,&page,&offset,&recnum);
	} while(loc && loc!=appts[index][n].loc);
	if(!loc)
	{
		beep();
		beep();
		beep();
		return 0;
	}
	x=y=currow=curcol=0;
	putstring("Are you sure you wish to delete this entry? (y/[N])");
	if(ozgetch()!='y') return 0;
	switch(appts[index][n].type)
	{
		case TYPE_TODO: typenum=TYPENUM_TODO; break;
		case TYPE_ANNIVERSARY: typenum=TYPENUM_ANNIVERSARY; break;
		case TYPE_SCHEDULE: typenum=TYPENUM_SCHEDULE; break;
	}
	if(ozunlink(typenum,recnum)) beep();
	return 1;
}

int repeat_appt(long startdate, int n, int delmode)
{
	static int index,appt;
	static int num,i;
	static int mode, c;
	static unsigned year, month, day;
	static unsigned delta;
	static byte typenum;
	reset_term();
	for(i=0;i<NUMDAYS;i++)
	{
		if(n<num_appts[i])
		{
			appt=n;
			index=i;
			break;
		}
		n-=num_appts[i];
	}
	if(i==NUMDAYS)
	{
		beep();
		beep();
		beep();
		return 0;
	}
	x=y=currow=curcol=0;
	year=theyear[index];
	day=theday[index];
	month=themonth[index];
	dumpday(y,startdate,index,appt,1);
	entrytype=appts[index][n].type;
	y+=PROP_FONT_HEIGHT*2;
	mode=UNSPECIFIED;
	if(delmode)
	{
		int cx,cy,cr,cc;
		cx=x;
		cy=y;
		cr=currow;
		cc=curcol;
		switch(appts[index][n].type)
		{
			case TYPE_TODO: typenum=TYPENUM_TODO; break;
			case TYPE_ANNIVERSARY: typenum=TYPENUM_ANNIVERSARY; break;
			case TYPE_SCHEDULE: typenum=TYPENUM_SCHEDULE; break;
		}
		putstring("Press Y or O to delete just this one, R for repeating\r\n"
				  "deletion, or any other key to cancel.\r\n");
		if(c=='y' || c=='O')
		{
			if(ozunlink(typenum,recnum)) beep();
			return 1;
		}
		else if(c=='r')
			x=cx; y=cy; currow=cr; curcol=cc;
		else return 0;
	}
	if(mode==UNSPECIFIED)
	{
		if(entrytype!=TYPE_ANNIVERSARY)
			putstring("Repeat (D)aily, (W)eekly, Every (T)wo weeks,\r\n"
				  "(M)onthly or (Y)early?  (Any other key to cancel.)\r\n");
		else
			putstring("Repeat (D)aily, (W)eekly, Every (T)wo weeks\r\n"
				  "or (M)onthly?  (Any other key to cancel.)\r\n");
		switch(mygetch())
		{
			case 'd':
					  putstring("Daily: (E)veryday or only (W)eekdays? [E]\r\n");
					  if((c=mygetch())=='w')
					  {
						putstring("Weekdays\r\n");
						mode=WEEKDAILY;
					  }
					  else if(c==KEY_LOWER_ESC || c==KEY_UPPER_ESC) return 0;
					  else
					  {
						putstring("Everyday\r\n");
						mode=DAILY;
					  }
					  break;
			case 'w': mode=WEEKLY;
					  putstring("Weekly\r\n");
					  break;
			case 't': mode=TWOWEEKLY;
					  putstring("Every two weeks\r\n");
					  break;
			case 'y': if(entrytype=TYPE_ANNIVERSARY);
					  mode=YEARLY;
					  putstring("Yearly\r\n");
					  break;
			case 'm': mode=MONTHLY_DATE_BEG;
					  putstring("Monthly\r\n");
					  break;
			default: return 0;
		}
		if(delmode)
			putstring("This one and how many more? (ESC to cancel) ");
		else
			putstring("How many times? (ESC to cancel) ");
		if(-1==(num=getnumber()) || num==0) return 0;
		putstring("\r\n");
		if((mode==YEARLY && num>100)||num>1000)
		{
			putstring("Sorry, that's too many.\r\n");
			putstring("Press a key to resume.\r\n");
			mygetch();
			return 0;
		}
		putstring("Are you sure? (Y/[N])");
		if(mygetch()!='y') return 0;
	}
	grabentry(appts[index][n].loc);
	delta=0;
	switch(mode)
	{
	   case DAILY: delta=1; break;
	   case WEEKDAILY:
		 index++;
		 while(num)
		 {
			int wd=(startdate+index)%7;
			if(wd==0 || wd==6)
			{
				index++;
				continue;
			}
			handleentry(startdate+index,delmode);
			index++;
			num--;
		 }
		 break;
	   case WEEKLY: delta=7; break;
	   case TWOWEEKLY: delta=14; break;
	   case YEARLY:
		 for(i=1;i<=num;i++)
		 {
		   if(existday(year+i,month,day))
			 handleentry(ymd_to_scalar(year+i,month,day),delmode);
		 }
		 break;
	   case MONTHLY_DATE_BEG:
		 month++;
		 for(i=0;i<num;i++,month++)
		 {
			if(month>12)
			{
				year++;
				month=1;
			}
			if(existday(year,month,day))
			  handleentry(ymd_to_scalar(year,month,day),delmode);
		 }
	}
	if(delta)
		for(i=1;i<=num;i++)
			handleentry(startdate+index+((long)i)*delta,delmode);
	return 1;
}
