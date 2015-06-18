#include <oz.h>
#include <ctype.h>
#include <string.h>

#include "scaldate.h"
#include "viewweek.h"

/* #define DEBUG */

extern byte reverse,underline;
extern byte curcol,currow;

enum repeat_types { CANCEL, DAILY, WEEKDAILY, WEEKLY, TWOWEEKLY,
	MONTHLY_DAY_START, MONTHLY_DAY_END, MONTHLY_WDAY_START, MONTHLY_WDAY_END,
	YEARLY };


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
	static unsigned c;
    while((c=ozgetchblank())==KEY_LEFT_SHIFT || c==KEY_RIGHT_SHIFT);
	if(c==KEY_MYPROGRAMS || c==KEY_CALENDAR || c==KEY_MEMO
	   || c==KEY_TELEPHONE || c==KEY_MAIN) ozexitto(c);
	return c;
}

byte entrytype;

byte monthlengths[]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

byte x,y;

int getnumber(void)
{
	static unsigned k;
	static int n;
#define MAXDIGITS 5
	static byte positions[MAXDIGITS];
	static byte digits=0;
	positions[0]=x;
	n=digits=0;
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
				 {
					curcol--;
					termputch(' ');
					curcol--;
				 }
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
    ozopenfile(loc);
	for(l=0; l<1023; l++)
        if(-1!=(c=ozreadfilebyte()) && (c || l<textat))
        {
			*p++=c;
        }
		else break;
	*p++=0;
/*	  for(l=0; l<1000; l++) *p++='Z'; */
	entry_len=p-entry_space;
	entry_space[4]=entry_space[5]=0xff;
    ozclosefile();
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

void datetostring(char *_s,long date)
{
    static unsigned y,m,d;
    register char *s=_s;
    scalar_to_ymd(date,&y,&m,&d);
    strcpy(s,utoa_0n(y,4));
    strcpy(s+4,utoa_0n(m,2));
    strcpy(s+6,utoa_0n(d,2));
    s[8]=0;
}

void putstring(char *s0)
{
	register char *s=s0;
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

void putnumberth(unsigned number)
{
	register char *p;
	if(number==0)
	{
#ifdef ITALIAN
        if(propfont)putstring("0\xB0"); else putstring("0\xF8");
#else
		putstring("0th");
#endif
	}
	p=utoa_0n(number,5);
	while(*p=='0') p++;
	putstring(p);
#ifdef ITALIAN
    if(propfont) putstring("\xB0"); else putstring("\xF8");
#else
	if(number>3 && number<21) putstring("th");
	 else
	switch(number%10)
	{
		case 1: putstring("st"); break;
		case 2: putstring("nd"); break;
		case 3: putstring("rd"); break;
	   default: putstring("th"); break;
	}
#endif
}

void dupentry(long date)
{
	static unsigned y,m,d;
	register char *s;
	static unsigned k;
	static int date_base;
	static byte writeentrytype;
	if(ozkeyhit())
	{
		k=mygetch();
		if(k==KEY_LOWER_ESC || k==KEY_UPPER_ESC) exit(0);
		ozungetch(k);
	}
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
#ifdef ITALIAN
	putstring(utoa_0n(d,2));
	putstring("/");
	putstring(utoa_0n(m,2));
#else
	putstring(s);
#endif
	strncpy(date_base+6+entry_space,s=utoa_0n(d,2),2);
#ifndef ITALIAN
	putstring("/");
	putstring(s);
#endif
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
	s=(char*)entry_space;
	s[4]=0xFF;	/* no ID assigned yet */
	s[5]=0xFF;
	s[6]=0xD0;	/* never synchronized */
	ozwritefile(s,entry_len,writeentrytype);
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
	static int appt,index,i;
	static unsigned page,offset,recnum,loc;
	static byte typenum;
	page=offset=0;
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
#ifndef ITALIAN
	putstring("Are you sure you wish to delete this entry? (y/[N])");
	if(mygetch()!='y') return 0;
#else /* ITALIAN */
	putstring("Sei sicuro di cancellare questa voce ? (s/[N])");
	if(mygetch()!='s') return 0;
#endif /* ITALIAN */
	switch(appts[index][n].type)
	{
		case TYPE_TODO: typenum=TYPENUM_TODO; break;
		case TYPE_ANNIVERSARY: typenum=TYPENUM_ANNIVERSARY; break;
		case TYPE_SCHEDULE: typenum=TYPENUM_SCHEDULE; break;
	}
    ozunlink(typenum,recnum);
	return 1;
}

enum repeat_types getmonthlymode(byte day,byte month,unsigned year,byte wd)
{
	  static byte n;
#ifdef ITALIAN
      putstring("(1) Ogni ");
#else
      putstring("(1) Every ");
#endif
	  putnumberth(day);
#ifdef ITALIAN
	  putstring(" giorno del mese\r\n"
				"(2) Ogni ");
#else
	  putstring(" day of month\r\n"
				"(2) Every ");
#endif
	  n=monthlength(month,year)-day+1;
	  if(n!=1)
	  {
		putnumberth(n);
		putstring(" ");
	  }
#ifdef ITALIAN
	  putstring("ultimo giorno del mese\r\n"
				"(3) Ogni ");
#else
	  putstring("last day of month\r\n"
				"(3) Every ");
#endif
	  putnumberth((day-1)/7+1);
	  putstring(" ");
	  putstring(days[wd]);
#ifdef ITALIAN
	  putstring(" del mese\r\n"
				"(4) Ogni ");
#else
	  putstring(" of month\r\n"
				"(4) Every ");
#endif
	  n=(n-1)/7+1;
	  if(n!=1)
	  {
		putnumberth(n);
		putstring(" ");
	  }
#ifdef ITALIAN
	  putstring("ultimo ");
#else
	  putstring("last ");
#endif
	  putstring(days[wd]);
#ifndef ITALIAN
	  putstring(" of month ? ");
#else
	  putstring(" del mese ? ");
#endif
	  switch(mygetch())
	  {
		case '1': putstring("1\r\n"); return MONTHLY_DAY_START;
		case '2': putstring("2\r\n"); return MONTHLY_DAY_END;
		case '3': putstring("3\r\n"); return MONTHLY_WDAY_START;
		case '4': putstring("4\r\n"); return MONTHLY_WDAY_END;
		default: return CANCEL;
	  }
}

long wdayfromstart(byte n,byte wday,byte month,unsigned year)
{
	static long start;
	static byte i,ml;
	ml=monthlength(month,year);
	start=ymd_to_scalar(year,month,1);
	for(i=0;i<ml;i++,start++)
		if(start%7==wday)
		{
			n--;
			if(n==0) return start;
		}
	return 0;
}

long wdayfromend(byte n,byte wday,byte month,unsigned year)
{
	static long start;
	static byte i,ml;
	start=ymd_to_scalar(year,month,ml=monthlength(month,year));
	for(i=0;i<ml;i++,start--)
		if(start%7==wday)
		{
			n--;
			if(n==0) return start;
		}
	return 0;
}

int repeat_appt(long startdate, int n)
{
	static int index,appt;
	static byte d,fe;
	static int num,i;
	static int mode, c;
	static unsigned year, month, day;
	static unsigned delta;
	static long dd;
	static byte wd;
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
	x=currow=curcol=0;
	year=theyear[index];
	day=theday[index];
	month=themonth[index];
	wd=(startdate+index)%7;
	dumpday(0,startdate,index,appt,1);
	entrytype=appts[index][n].type;
	y=PROP_FONT_HEIGHT*2;

#ifndef ITALIAN
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
		case 'y': if(entrytype==TYPE_ANNIVERSARY);
				  mode=YEARLY;
				  putstring("Yearly\r\n");
				  break;
		case 'm': x=currow=curcol=0;
				  reset_term();
				  dumpday(0,startdate,index,appt,1);
				  y=PROP_FONT_HEIGHT*2;
				  mode=getmonthlymode(day,month,year,wd);
				  if(mode==CANCEL) return 0;
				  break;
		default: return 0;
	}
#else /* ITALIAN */
	if(entrytype!=TYPE_ANNIVERSARY)
	  putstring("Ripeti: (G)iorn., (S)ettim., Ogni (D)ue settimane,\r\n"
			  "(M)ensilm. o (A)nnualm.?  (Un tasto per uscire)\r\n");
	else
	  putstring("Ripeti: (G)iorn., (S)ettim., Ogni (D)ue settimane,\r\n"
			  "o (M)ensilm.  (Un tasto per uscire)\r\n");

	switch(mygetch())
	{
		case 'g':
				  putstring("Giorn: (O)gni giorno o solo (L)avorativi ? [O]\r\n");
				  if((c=mygetch())=='l')
				  {
					putstring("Lavorativi\r\n");
					mode=WEEKDAILY;
				  }
				  else if(c==KEY_LOWER_ESC || c==KEY_UPPER_ESC) return 0;
				  else
				  {
					putstring("Ogni giorno\r\n");
					mode=DAILY;
				  }
				  break;
		case 's': mode=WEEKLY;
				  putstring("Settimanale\r\n");
				  break;
		case 'd': mode=TWOWEEKLY;
				  putstring("Ogni due settimane\r\n");
				  break;
		case 'a': if(entrytype==TYPE_ANNIVERSARY);
				  mode=YEARLY;
				  putstring("Annuale\r\n");
				  break;
		case 'm': x=currow=curcol=0;
				  reset_term();
				  dumpday(0,startdate,index,appt,1);
				  y=PROP_FONT_HEIGHT*2;
				  mode=getmonthlymode(day,month,year,wd);
				  if(mode==CANCEL) return 0;
				  break;
		default: return 0;
	}
#endif /* ITALIAN */

	if(mode==MONTHLY_DAY_START || mode==MONTHLY_DAY_END ||
	mode==MONTHLY_WDAY_START || mode==MONTHLY_WDAY_END)
#ifdef ITALIAN
		putstring("Quanti mesi ? (ESC per uscire) ");
#else
		putstring("How many months? (ESC to cancel) ");
#endif
	else
#ifdef ITALIAN
		putstring("Quante volte ? (ESC per uscire) ");
#else
		putstring("How many times? (ESC to cancel) ");
#endif

	if(-1==(num=getnumber()) || num==0) return 0;
	putstring("\r\n");
	if((mode==YEARLY && num>100)||num>1000)
	{
#ifndef ITALIAN
		putstring("Sorry, that's too many.\r\n");
		putstring("Press a key to resume.\r\n");
#else /* ITALIAN */
		putstring("Mi dispiace sono troppe.\r\n");
		putstring("Premi un tasto per continuare ...\r\n");
#endif /* ITALIAN */

		mygetch();
		return 0;
	}
#ifdef DEBUG
	putstring(itoa(num));
#endif
#ifndef ITALIAN
	putstring("Are you sure? (Y/[N]) (You'll have to delete them one\r\n"
			  "by one if you generate them and change your mind.)\r\n");
	if(mygetch()!='y') return 0;
#else /* ITALIAN */
	putstring("Sei sicuro ? (S/[N]) (Devi cancellarle una ad\r\n"
			  "una se generi e poi cambi idea !)\r\n");
	if(mygetch()!='s') return 0;
#endif /* ITALIAN */

	grabentry(appts[index][n].loc);
	delta=0;
	switch(mode)
	{
	   case DAILY: delta=1; break;
	   case WEEKDAILY:
		 index++;
		 while(num)
		 {
			wd=(startdate+index)%7;
			if(wd==0 || wd==6)
			{
				index++;
				continue;
			}
			dupentry(startdate+index);
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
			 dupentry(ymd_to_scalar(year+i,month,day));
		 }
		 break;
	   case MONTHLY_DAY_START:
		 month++;
		 for(i=0;i<num;i++,month++)
		 {
			if(month>12)
			{
				year++;
				month=1;
			}
			if(existday(year,month,day))
			  dupentry(ymd_to_scalar(year,month,day));
		 }
		 break;
	   case MONTHLY_DAY_END:
		fe=monthlength(month,year)-day;
		month++;
		for(i=0;i<num;i++,month++)
		{
			if(month>12)
			{
				year++;
				month=1;
			}
			if((d=monthlength(month,year)-fe) >= 1)
			  dupentry(ymd_to_scalar(year,month,d));
		}
		break;
	   case MONTHLY_WDAY_START:
		n=(day-1)/7+1;
		month++;
		for(i=0;i<num;i++,month++)
		{
			if(month>12)
			{
				year++;
				month=1;
			}
			if(dd=wdayfromstart(n,wd,month,year))
			 dupentry(dd);
		}
		break;
	   case MONTHLY_WDAY_END:
		n=(monthlength(month,year)-day)/7+1;
		month++;
		for(i=0;i<num;i++,month++)
		{
			if(month>12)
			{
				year++;
				month=1;
			}
			if(dd=wdayfromend(n,wd,month,year))
			 dupentry(dd);
		}
		break;
	}
	if(delta)
		for(i=1;i<=num;i++)
			dupentry(startdate+index+((long)i)*delta);
	return 1;
}
