#include <oz.h>

#define AM_CHAR 0xC0
#define PM_CHAR 0xC1
#include "viewweek.h"

extern byte reverse,underline;
extern byte curcol,currow;

byte ozputch0(byte x,byte y,byte c)
{
    if(c==' ') return x+5;
      else return ozputch(x,y,c);
}

void reverselines(byte y,byte n);

char *itoa(int i);

extern byte propfont;
extern byte mainheader;
extern char alignat;
extern byte viewempty;
extern char viewheaders;
extern byte view_todos;
extern byte view_annivs;
extern byte view_appts;
extern byte overflowed;
extern byte repeatchooser;
extern byte _24hr;
extern byte silent;
extern int found;
extern byte time_indent;

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

int dumpdate(int x, int y, long start, int index, int longform);

static int x,y;

byte gettwodigits(char *s)
{
    return (s[0]-'0')*10+(s[1]-'0');
}

void minihead(long start, int index, int i)
{
            switch(i)
            {
                int d;
                case 0:
                    d=(start+index)%7;
                    if(propfont)
                    {
                        char day[4];
                        strncpy(day,days[d],3);
                        day[3]=0;
                        ozputs(0,y,day);
                        reverse2bytes(y,PROP_FONT_HEIGHT);
                        x=NOHEAD_INDENT;
                    }
                    else
                    {
                        reverse=1;
                        ttyprint(days[d][0]);
                        ttyprint(days[d][1]);
                        ttyprint(days[d][2]);
                        reverse=0;
                        ttyprint(' ');
                    }
                    break;
                case 1:
                    if(propfont)
                    {
                        ozputs(x,y,itoa(theday[index]));
                        x=NOHEAD_INDENT;
                    }
                    else
                    {
                        termputs(itoa(theday[index]));
                        while(curcol<3) ttyprint(' ');
                        ttyprint(' ');
                    }
                    break;
                case 2:
                    if(propfont)
                    {
                        char month[4];
                        strncpy(month,months[themonth[index]-1],3);
                        month[3]=0;
                        ozputs(0,y,month);
                        x=NOHEAD_INDENT;
                    }
                    else
                    {
                        ttyprint(months[themonth[index]-1][0]);
                        ttyprint(months[themonth[index]-1][1]);
                        ttyprint(months[themonth[index]-1][2]);
                        ttyprint(' ');
                    }
                    break;
                default:
                    if(propfont)
                        x=NOHEAD_INDENT;
                    else
                        termputs("    ");
                    break;
            }
}

byte puthour(byte x,char *_hour)
{
    static byte hour;
    static char hour1;
    if(*_hour==' ')
    {
        if(propfont)
        {
          PROPPUTCH0(' ');
          PROPPUTCH0(' ');
        }
        else
        {
          ttyprint(' ');
          ttyprint(' ');
        }
        return x;
    }
    hour=gettwodigits(_hour);
    if(!_24hr)
    {
      if(hour>12)
         hour-=12;
      else if(hour==0)
         hour=12;
    }
    hour1=hour/10+'0';
    if(!_24hr && hour1=='0') hour1=' ';
    if(propfont)
    {
        PROPPUTCH0(hour1);
        PROPPUTCH(hour%10+'0');
        return x;
    }
    else
    {
        ttyprint(hour1);
        ttyprint(hour%10+'0');
        return 0;
    }
}

byte putampm(byte x,char *_hour)
{
    static byte hour,pm;
    if(_24hr) return x;
    if(*_hour==' ')
    {
        if(propfont)
          PROPPUTCH0(' ');
        else
          ttyprint(' ');
        return x;
    }
    hour=gettwodigits(_hour);
    if(hour>=12)
        pm=1;
    else
        pm=0;
    if(propfont)
    {
            ozfont(FONT_OZ_NORMAL);
            PROPPUTCH(AM_CHAR+pm);
            ozfont(FONT_PC_NORMAL);
            return x;
    }
    else
    {
            ttyprint(pm?'p':'a');
            return 0;
    }
}


int dumpday(int _y, long start, int index, int from, int maxn)
{
    static int i;
    static char *b;
    static byte x0;
    register char *b2;
    if((!viewempty || !viewheaders || repeatchooser) && !num_appts[index]) return 0;
    if(index>=NUMDAYS || maxn==0) return 0;
    x=0; y=_y;
    if(from==-1)
    {
      reverse=1;
#ifndef OLDSTYLE
      if(propfont) x=ozputch(x,y,' '); else termputch(' ');
#endif
      x=dumpdate(x,y,start,index,1);
      if(propfont)
      {
        PROPPUTS(": ");
#ifndef ITALIAN
        PROPPUTS(num_appts[index]?itoa(num_appts[index]):"no");
        ozputs(x,y,num_appts[index]==1?" entry.":" entries.");
#else /* ITALIAN */
        PROPPUTS(num_appts[index]?itoa(num_appts[index]):"nessun");
        ozputs(x,y,num_appts[index]==1?" campo.":" campi.");
#endif /* ITALIAN */
        reverselines(y,PROP_FONT_HEIGHT);
        x=0; y+=PROP_FONT_HEIGHT;
      }
      else
      {
          termputs(": ");
          termputs(num_appts[index]?itoa(num_appts[index]):"no");
#ifndef ITALIAN
          termputs(num_appts[index]==1?" entry.":" entries.");
#else /* ITALIAN */
          termputs(num_appts[index]==1?" campo.":" campi.");
#endif /* ITALIAN */
          while(curcol<WIDTH) ttyprint(' ');
      }
      reverse=0;
    }
    if(num_appts[index]-from>maxn) maxn=from+maxn; else maxn=num_appts[index];
    for(i=from<0?0:from; i<maxn; i++)
    {
        if(repeatchooser)
        {
            dumpdate(x,y,start,index,1);
            if(propfont)
            {
                y+=PROP_FONT_HEIGHT;
                x=0;
            }
            else
            {
                currow++;
                curcol=0;
            }
        }
        else
        if(!viewheaders) minihead(start,index,i);
        b=ozloadcluster(appts[index][i].loc);
        switch(appts[index][i].type)
        {
            case TYPE_SCHEDULE:
                x0=x;
                x=puthour(x,b+24);
                if(propfont)
                {
                    PROPPUTCH(':');
                    PROPPUTCH0(b[26]);
                    PROPPUTCH0(b[27]);
                    x=putampm(x,b+24);
                    PROPPUTCH('-');
                    x=puthour(x,b+28);
                    PROPPUTCH(':');
                    PROPPUTCH0(b[30]);
                    PROPPUTCH0(b[31]);
                    x=putampm(x,b+28);
                    x=x0+time_indent;
                }
                else
                {
                    ttyprint(':');
                    ttyprint(b[26]);
                    ttyprint(b[27]);
                    putampm(0,b+24);
                    ttyprint('-');
                    puthour(0,b+28);
                    ttyprint(':');
                    termputs(b+30);
                    putampm(0,b+28);
                    ttyprint(' ');
                }
                b2=b+38;
                break;
            case TYPE_ANNIVERSARY:
                if(propfont)
                {
                    ozputs(x,y,"Anniv.");
                    x+=time_indent;
                }
                else
                {
                    underline=1;
#ifndef ITALIAN
                    termputs("Anniversary");
#else /* ITALIAN */
                    termputs("Anniversario");
#endif /* ITALIAN */
                    underline=0;
                    if(!_24hr)
                        termputs("  ");
                    ttyprint(' ');
                }
                b2=b+2+13+5;
                break;
            case TYPE_TODO:
                if(propfont)
                {
                    int x0=x;
#ifndef ITALIAN
                    PROPPUTS("TO DO [");
#else /* ITALIAN */
                    PROPPUTS("DAFARE[");
#endif /* ITALIAN */
                    PROPPUTCH(b[2+13+2+9]);
                    ozputch(x,y,']');
                    x=x0+time_indent;
                }
                else
                {
                    underline=1;
#ifndef ITALIAN
                    termputs("TO DO [");
#else /* ITALIAN */
                    termputs("DAFARE[");
#endif /* ITALIAN */
                    ttyprint(b[2+13+2+9]);
                    termputs("]");
                    underline=0;
                    termputs("   ");
                    if(!_24hr) termputs("  ");
                }
                b2=b+2+13+2+9+4;
                break;
        }
        if(propfont)
            PROPPUTS(b2);
        else
            while(*b2 && b2<b+64 && curcol<WIDTH)
              ttyprint(*b2++);
        if(b[1]!='\xFF' || b[0]!='\xFF')
        {
            unsigned loc2;
            b2=2+ozloadcluster(loc2=ozinclocation(appts[index][i].loc));
            if(propfont)
            {
                if(x<SCREEN_WIDTH && !*(ozputsgetend())) PROPPUTS(b2);
                if(x<SCREEN_WIDTH && (b2[-2]!='\xFF' || b2[-1]!='\xFF')
                  && !*(ozputsgetend()))
                  ozputs(x,y,2+ozloadcluster(ozinclocation(loc2)));
            }
            else
              while(*b2 && curcol<WIDTH)
                ttyprint(*b2++);
        }
        if(propfont)
        {
            x=0;
            y+=PROP_FONT_HEIGHT;
        }
        else curcol=WIDTH;
    }
    return i-from;
}

