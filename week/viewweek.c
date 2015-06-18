#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "scaldate.h"

/* #define PROFILER /* */

/* #define DEBUG 1 /* */

#include "viewweek.h"

extern byte reverse,underline;
extern byte curcol,currow;
static char start_date_string[9];
static char end_date_string[9];

char *itoa(int i);

unsigned help=5;
byte propfont=0;
byte mainheader=1;
long startdate;
char alignat=-1;
byte viewempty=1;
char viewheaders=0;
byte view_todos=1;
byte view_annivs=1;
byte view_appts=1;
byte overflowed=0;
byte silent=0;
byte repeatchooser=0;
byte curfontheight;
byte height;
byte _24hr=1;
byte time_indent=TIME_INDENT;
int found;

int dumpday(int y, long startdate, int index, int from, int maxn);

typedef struct
{
    unsigned loc;
    char type;
} appts_t;

appts_t appts[NUMDAYS][MAXAPPTS];
int num_appts[NUMDAYS];
unsigned theday[NUMDAYS];
unsigned themonth[NUMDAYS];
unsigned theyear[NUMDAYS];

void reverselines(byte y,byte n);
#ifdef ITALIAN
char *months[]={ "Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio",
    "Giugno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre",
    "Dicembre" };
#endif /* ITALIAN */

#ifndef ITALIAN
char *months[]={ "January", "February", "March", "April", "May",
    "June", "July", "August", "September", "October", "November",
    "December" };

char *days[]={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday" };
#else /* ITALIAN */
char *days[]={ "Domenica", "Lunedi", "Martedi", "Mercoledi", "Giovedi", "Venerdi",
    "Sabato" };
#endif /* ITALIAN */

#ifndef ITALIAN
char *shortdays[]={ "Sun.", "Mon.", "Tue.", "Wed.", "Thurs.", "Fri.",
    "Sat." };
#else /* ITALIAN */
char *shortdays[]={ "Dom.", "Lun.", "Mar.", "Mer.", "Gio.", "Ven.",
    "Sab." };
#endif /* ITALIAN */

long
string_to_scalar(char *d)
{
    char date[9];
    unsigned year,month,day;
    memcpy(date,d,9);
    day=atoi(date+6);
    date[6]=0;
    month=atoi(date+4);
    date[4]=0;
    year=atoi(date);
    return ymd_to_scalar(year,month,day);
}

void mybeep(void)
{
    if(!silent) beep();
}

void highlight(byte cursor)
{
    reverselines(cursor*2*curfontheight,curfontheight*2);
}

int dumpdate(int x, int y, long startdate, int index, int longform)
{
   if(!propfont)
   {
      termputs(longform?days[(startdate+index) % 7]:
               shortdays[(startdate+index) % 7]);
      termputs(", ");
#ifdef ITALIAN
      termputs(itoa(theday[index]));
      ttyprint(' ');
#endif /* ITALIAN */
      termputs(months[themonth[index]-1]);
      ttyprint(' ');
#ifndef ITALIAN
      termputs(itoa(theday[index]));
      termputs(", ");
#endif /* not ITALIAN */
      termputs(itoa(theyear[index]));
      return 0;
   }
   else
   {
      x=ozputs(x,y,longform?days[(startdate+index) % 7]:
               shortdays[(startdate+index) % 7]);
      x=ozputs(x,y,", ");
#ifdef ITALIAN
      x=ozputs(x,y,itoa(theday[index]));
      x=ozputs(x,y," ");
#endif /* ITALIAN */
      x=ozputs(x,y,months[themonth[index]-1]);
      x=ozputch(x,y,' ');
#ifndef ITALIAN
      x=ozputs(x,y,itoa(theday[index]));
      x=ozputs(x,y,", ");
#endif /* not ITALIAN */
      x=ozputs(x,y,itoa(theyear[index]));
      return x;
   }
}

void printheader(int y, long startdate)
{
    static byte x;
    reverse=1;
#ifndef OLDSTYLE
    if(propfont) x=ozputch(0,y,' ');
      else termputch(' ');
#else
    x=0;
#endif
    x=dumpdate(x,y,startdate,0,0);
    if(propfont) x=ozputs(x,y," - ");
     else termputs(" - ");
    x=dumpdate(x,y,startdate,NUMDAYS-1,0);
    if(!propfont) while(curcol<WIDTH) termputch(' ');
     else reverselines(y,PROP_FONT_HEIGHT);
    reverse=0;
}


int dumpat(int y,long startdate, int loc, int num)
{
    static int vh,ap,day,i;
    static byte putheader;
    vh=viewheaders && !repeatchooser;
    ap= -vh;
    day=0;
    putheader=!repeatchooser && mainheader;
    i=putheader;
    if(!found && !repeatchooser)
    {
        printheader(y+0,startdate);
        if(propfont)
#ifndef ITALIAN
            ozputs(0,y+PROP_FONT_HEIGHT,"No appointments found.");
#else /* ITALIAN */
            ozputs(0,y+PROP_FONT_HEIGHT,"Non trovo appuntamenti.");
#endif /* ITALIAN */
        else
#ifndef ITALIAN
            termputs("No appointments found.\r\n");
#else /* ITALIAN */
            termputs("Non trovo appuntamenti.\r\n");
#endif /* ITALIAN */
        return 2;
    }
    if(loc) putheader=0;
    while(i<loc && day<NUMDAYS)
    {
        if((!viewempty || !vh) && !num_appts[day])
        {
            day++;
            continue;
        }
        i++;
        ap++;
        if(ap>=num_appts[day])
        {
            ap=(-vh);
            day++;
        }
/*
        termputs("ap=");
        termputs(itoa(ap));
        termputs(" i=");
        termputs(itoa(i));
        termputs(" day=");
        termputs(itoa(day));
        termputs(" loc=");
        termputs(itoa(loc));
        termputs(" num_appts[day]=");
        termputs(itoa(num_appts[day]));
        termputs("\r\n");
*/
    }
    i=0;
    if(putheader)
    {
        printheader(y+0,startdate);
        i++;
    }
    while(i<num && day<NUMDAYS)
    {
        i+=dumpday(y+i*(1+repeatchooser)*PROP_FONT_HEIGHT,startdate,day,ap,num-i);
        day++;
        ap=(-vh);
    }
    return i;
}

int
mycompare(void *_e1, void *_e2)
/* appts_t *e1, appts_t *e2) */
#define e1 ((appts_t *)_e1)
#define e2 ((appts_t *)_e2)
{
    static char buf1[64];
    register char *buf2;
/*    ttyprint((*e1).type); */
    if(e1->type != e2->type)
    {
      if(e1->type==TYPE_SCHEDULE) return 1;
       else if(e2->type==TYPE_SCHEDULE) return -1;
        else if(e1->type==TYPE_TODO) return -1;
         else return -1;
    }
    switch(e1->type)
    {
        case TYPE_SCHEDULE:
             strncpy(buf1,13+9+2+ozloadcluster(e1->loc),9);
             return strncmp(buf1,13+9+2+ozloadcluster(e2->loc),8);
        case TYPE_ANNIVERSARY:
             memcpy(buf1,2+13+9+ozloadcluster(e1->loc),64-(2+13+9));
             return strncmp(buf1,2+13+9+ozloadcluster(e2->loc),64-(2+13+9));
        case TYPE_TODO:
             memcpy(buf1,2+13+2+9+ozloadcluster(e1->loc),64-(2+13+2+9));
             buf2=2+13+2+9+ozloadcluster(e2->loc);
             if(*buf1!=*buf2) return *buf1-*buf2; /* priority */
              else return strncmp(buf1+4,buf2+4,64-(2+13+2+9+4));
    }
}
#undef e1
#undef e2

void load_todos()
{
    static unsigned page,offset,loc,recnum;
    register char *cluster;
    static int i;
    static long scaldate;
        page=offset=0;
        while(loc=ozfindnext(TYPE_TODO,&page,&offset,&recnum))
        {
            cluster=ozloadcluster(loc);
            if(' '==cluster[2+13+2] || 2==cluster[2+13+2+9+2]) continue;
               /* undated or done*/
            if(strcmp(start_date_string,cluster+2+13+2)>0
             || strcmp(end_date_string,cluster+2+13+2)<=0) continue;
            scaldate=string_to_scalar(cluster+2+13+2);
/*            if(scaldate<startdate || scaldate>=startdate+NUMDAYS) continue; */
            i=(int)(scaldate-startdate);
            if(num_appts[i]>=MAXAPPTS)
            {
                overflowed=1;
                continue;
            }
            appts[i][num_appts[i]].type=TYPE_TODO;
            appts[i][num_appts[i]++].loc=loc;
            found++;
            ttyprint('.');
        }
    }

void load_annivs()
{
    static unsigned page,offset,loc,recnum;
    static int i;
    register char *cluster;
    page=offset=0;
    while(loc=ozfindnext(TYPE_ANNIVERSARY,&page,&offset,&recnum))
    {
        int d,m;
        cluster=ozloadcluster(loc);
        d=atoi(cluster+2+13+2);
        cluster[2+13+2]=0;
        m=atoi(cluster+2+13);
        for(i=0;i<NUMDAYS;i++)
            if(theday[i]==d && themonth[i]==m) break;
        if(i==NUMDAYS) continue;
        if(num_appts[i]>=MAXAPPTS)
        {
            overflowed=1;
            continue;
        }
        appts[i][num_appts[i]].type=TYPE_ANNIVERSARY;
        appts[i][num_appts[i]++].loc=loc;
        found++;
        ttyprint('.');
    }
}

void load_appts()
{
    static unsigned page,offset,loc,recnum;
    static int i;
    register char *cluster;
    long scaldate;
#ifdef DEBUG
    int pos=0;
#endif
    page=offset=0;
    while(loc=ozfindnext(TYPE_SCHEDULE,&page,&offset,&recnum))
    {
        cluster=ozloadcluster(loc);
        if(strcmp(start_date_string,cluster+2+13)>0
             || strcmp(end_date_string,cluster+2+13)<=0) continue;
        scaldate=string_to_scalar(cluster+2+13);
/*        if(scaldate<startdate || scaldate>=startdate+NUMDAYS) continue; */
        i=(int)(scaldate-startdate);
        if(num_appts[i]>=MAXAPPTS)
        {
            overflowed=1;
            continue;
        }
        appts[i][num_appts[i]].type=TYPE_SCHEDULE;
        appts[i][num_appts[i]++].loc=loc;
        found++;
        ttyprint('.');
    }
#ifdef DEBUG
        termputs("Press any key to continue");
        mygetch();
#endif
}

static byte doclear=0;

reload()
{
    static byte i;
    for(i=0; i<NUMDAYS; i++)
    {
      num_appts[i]=0;
      scalar_to_ymd(startdate+i,theyear+i,themonth+i,theday+i);
    }
    found=0;
    overflowed=0;
    if(doclear) reset_term(); else doclear=1;
#ifndef ITALIAN
    termputs("Scanning");
#else /* ITALIAN */
    termputs("Scansione");
#endif /* ITALIAN */
    if(view_todos) load_todos();
    if(view_annivs) load_annivs();
    if(view_appts) load_appts();
    termputs("\n\r");
    if(overflowed)
    {
#ifndef ITALIAN
        termputs("Too many entries on one day!\n\r");
        termputs("Press Q to quit, any other key to continue.\n\r");
#else /* ITALIAN */
        termputs("Troppi valori in un giorno !\n\r");
        termputs("Premi Q per uscire o spazio per continuare.\n\r");
#endif /* ITALIAN */
        if('q'==mygetch()) exit(0);
    }
#ifndef ITALIAN
    termputs("Sorting");
#else /* ITALIAN */
    termputs("Ordino ");
#endif /* ITALIAN */
    for(i=0;i<NUMDAYS;i++)
    {
        if(!num_appts[i]) continue;
        ttyprint('.');
        if(num_appts[i])
          bsort((void *)(appts[i]),num_appts[i],sizeof(appts[i][0]),mycompare);
    }
    termputs("\n\r");
}

int getnumlines(byte *adjheight)
{
    static int numlines;
    static int i;
    *adjheight=height;
    if(repeatchooser)
    {
      numlines=found;
      *adjheight=height/2;
    }
    else if(viewheaders && viewempty)
      numlines=found+NUMDAYS;
    else if(!viewheaders)
      numlines=found;
    else /* viewheaders && !viewempty */
    {
       numlines=0;
       for(i=0;i<NUMDAYS;i++)
         if(num_appts[i]) numlines+=1+num_appts[i];
    }
    if(!repeatchooser) numlines+=mainheader;
    return numlines;
}

void helpline(long startdate, int pos, byte adjheight, int numlines)
{
    static unsigned init_time;
    extern unsigned _oz1hz;
    init_time=_oz1hz;
    currow=HEIGHT-1;
    curcol=59-23;
    underline=1;
#ifndef ITALIAN
    termputs(" Press MENU for help. ");
#else /* ITALIAN */
    termputs(" Premi MENU per AIUTO ");
#endif /* ITALIAN */
    underline=0;
    while(!ozkeyhit() && init_time+help>_oz1hz)
      ozintwait();
    if(pos+adjheight<numlines) restorelower();
    if(propfont) clearbottom8();
    else
    {
      curcol=59-23;
      termputs("                      ");
    }
    curcol=0;
    if(pos+adjheight<=numlines)
    {
      /* FIX FOR REPEATCHOOSER */
      dumpat((height-1)*PROP_FONT_HEIGHT,startdate,pos+height-1,1);
      if(pos+adjheight<numlines) lowerarrow();
    }
}

void helpscreen(void)
{
    reset_term();
    /* 12345678901234567890123456789012345678901234567890123456789 */
    reverse=1;
    termputs(
#ifndef ITALIAN
      "If you like ViewWeek, you are invited, but not required, to\r\n"
      "donate $5 to any pro-life organization.\r\n");
#else /* ITALIAN */
      "Se ti piace ViewWeek, dona 10.000 lire ad un ente\r\n"
      "per la vita. (La donazione non e' obbligatoria)\r\n");
#endif /* ITALIAN */
    reverse=0;
    termputs(
#ifndef ITALIAN
      "UP/DOWN/PAGE-UP/PAGE-DOWN  scroll display\r\n"
      "LEFT/RIGHT / P/N  move start date by a day or week\r\n"
      "T/A/S  toggle viewing of TO DO/anniversary/schedule entries\r\n"
      "H/W/E  toggle viewing of day/week headers / empty entries\r\n"
      "R      delete/repeat mode (select, press enter or delete)\r\n"
      "F      change font size\r\n"
      "2      toggle 24 hour mode\r\n"
      "MENU/I this help screen\r\n"
      "ESC/Q  quit\r\n");
      termputs("Currently viewing: ");
#else /* ITALIAN */
      "SU/GIU'/PAG-SU/PAG-GIU  scorre display\r\n"
      "SINISTRA/DESTRA         cambia giorno inizio\r\n"
      "P/N    settimana precedente/successiva\r\n"
      "T/A/S  Visualizza: da fare/anniversari/appuntamenti\r\n"
      "H/W/E  Visualizza: giorno/intest. sett./campi vuoti\r\n"
      "R      cancella/modo ripeti (seleziona, premi invio o del)\r\n"
      "F      Cambia dimensione font\r\n"
      "MENU/I Visualizza questa lista comandi\r\n"
      "ESC/Q  Esci dal programma\r\n");
      termputs("    Vista attuale: ");
#endif /* ITALIAN */
      if(view_todos)
      {
         reverse=1;
#ifndef ITALIAN
         termputs("TO-DOs");
#else /* ITALIAN */
         termputs("DA FARE");
#endif /* ITALIAN */
         reverse=0;
         termputch(' ');
      }
      if(view_annivs)
      {
         reverse=1;
#ifndef ITALIAN
         termputs("anniversaries");
#else /* ITALIAN */
         termputs("Anniversari");
#endif /* ITALIAN */
         reverse=0;
         termputch(' ');
      }
      if(view_appts)
      {
         reverse=1;
#ifndef ITALIAN
         termputs("schedule entries");
#else /* ITALIAN */
         termputs("Appuntamenti");
#endif /* ITALIAN */
      }
      reverse=1;
      termputs(
#ifndef ITALIAN
        /* 12345678901234567890123456789012345678901234567890123456789 */
      "\r\nPress D for more info on donations or anything else to exit");
#else /* ITALIAN */
      "\r\n----qualsiasi tasto per uscire----");
#endif /* ITALIAN */
      reverse=0;
}

#ifndef ITALIAN
void donation_info()
{
    reset_term();
    termputs(
"If you like, you can send your $5 (or more, if you like)\r\n"
"donation to:\r\n"
"  BIRTHRIGHT OF PITTSBURGH, INC.\r\n"
"  201 South Craig Street\r\n"
"  Pittsburgh, PA 15213 U.S.A.\r\n"
"Birthright is a pregnancy crisis center helping pregnant\r\n"
"women in difficult circumstances.\r\n"
"Or else you can donate to your choice of pro-life\r\n"
"organization.  Any donation is optional, but thank you\r\n"
"very much if you do make one!\r\n\n");
    reverse=1;
    termputs("--Press any key to go back to ViewWeek--");
    reverse=0;
    mygetch();
}
#endif

main()
{
   static byte redraw=1;
   static unsigned c;
   static int pos,cursor;
   static byte adjheight;
   static int numlines;
   static unsigned int yr,mth,dy;
   extern unsigned _ozautoblanktime;
   _ozautoblanktime=15;
#ifdef PROFILER
   ozinitprofiler();
#endif
   reset_term();
   configure();
   if(propfont)
   {
      height=SCREEN_HEIGHT/PROP_FONT_HEIGHT;
      setscroll(PROP_FONT_HEIGHT,height);
   }
   else
   {
      height=HEIGHT;
      setscroll(FIXED_FONT_HEIGHT,height);
   }
    startdate=ymd_to_scalar(ozyear(),ozmonth(),ozday());
    if(startdate!=ymd_to_scalar(ozyear(),ozmonth(),ozday()))
       startdate=ymd_to_scalar(ozyear(),ozmonth(),ozday());
       /* fix possible bug */
    if(alignat!=-1) startdate-=((startdate-alignat) % 7);
RELOAD:
    datetostring(start_date_string,startdate);
    datetostring(end_date_string,startdate+NUMDAYS);
    reload();
    pos=0;
    cursor=0;
    curfontheight=propfont?PROP_FONT_HEIGHT:FIXED_FONT_HEIGHT;
    redraw=1;
    do
    {
       if(redraw)
       {
         if(_24hr)
            time_indent=TIME_INDENT;
         else
            time_indent=TIME_INDENT+10;
         numlines=getnumlines(&adjheight);
         reset_term();
         dumpat(0,startdate,pos,adjheight);
         redraw=0;
         if(pos>0) upperarrow();
         if(pos+adjheight<numlines) lowerarrow();
         if(repeatchooser) highlight(cursor);
         if(help && !repeatchooser)
         {
            helpline(startdate,pos,adjheight,numlines);
            help=0;
         }
       }
       c=mygetch();
       switch(c)
       {
         case KEY_UPPER_ENTER:
         case KEY_LOWER_ENTER:
         case '\r':
            if(!repeatchooser) break;
            redraw=1;
            if(repeat_appt(startdate,pos+cursor)) goto RELOAD;
            break;
         case '\8':
            if(!repeatchooser) break;
            redraw=1;
            if(killappt(pos+cursor)) goto RELOAD;
            break;
         case KEY_PAGEUP: if(pos<=0 && !(repeatchooser && cursor>0))
                             mybeep();
                          else
                          {
                           int pos0=pos;
                           pos-=adjheight-1;
                           if(pos<0) pos=0;
                           if(pos0==pos) cursor=0;
                           redraw=1;
                          }
                          break;
         case KEY_PAGEDOWN:
                          if(pos+adjheight>=numlines
                          && !(repeatchooser && cursor+pos<found-1)) mybeep();
                          else
                          {
                              static int pos0;
                              pos0=pos;
                              pos+=adjheight-1;
                              if(pos+adjheight>=numlines)
                              {
                                pos=numlines-adjheight;
                                if(pos<0) pos=0;
                              }
                              if(pos0==pos) cursor=adjheight-1;
                              if(cursor+pos>=numlines)
                                cursor=numlines-pos-1;
                              redraw=1;
                          }
                          break;
         case KEY_UP: if(pos<=0 && !(repeatchooser && cursor>0))
                      {
                        mybeep();
                        break;
                      }
                      if(!repeatchooser || cursor==0)
                      {
                          if(repeatchooser) highlight(cursor);
                          pos--;
                          restoreupper();
                          restorelower();
                          ttyscrolldown();
                          if(repeatchooser) ttyscrolldown();
                          curcol=currow=0;
                          dumpat(0,startdate,pos,1);
                          lowerarrow();
                          if(pos>0) upperarrow();
                          if(repeatchooser) highlight(cursor);
                      }
                      else
                      {
                          highlight(cursor);
                          cursor--;
                          highlight(cursor);
                      }
                      break;
         case KEY_DOWN:
                      if(pos+1+adjheight>numlines && !(repeatchooser
                        && cursor<numlines-pos-1))
                      {
                         mybeep();
                         break;
                      }
                      if(!repeatchooser || cursor==adjheight-1)
                      {
                        if(repeatchooser) highlight(cursor);
                        pos++;
                        restorelower();
                        ttyscroll();
                        if(repeatchooser) ttyscroll();
                        currow=(repeatchooser+1)*(adjheight-1);
                        curcol=0;
                        dumpat(currow*PROP_FONT_HEIGHT,startdate,pos+adjheight-1,1);
                        upperarrow();
                        if(pos+adjheight<numlines) lowerarrow();
                        if(repeatchooser)
                            highlight(cursor);
                      }
                      else
                      {
                        highlight(cursor);
                        cursor++;
                        highlight(cursor);
                      }
                      break;
         case KEY_LEFT: startdate--; goto RELOAD;
         case KEY_RIGHT: startdate++; goto RELOAD;
         case 'z':
                   ozsnap();
                   ozsnap();
                   break;
         case 'w':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }
                   mainheader=!mainheader; redraw=1; pos=0; break;
         case 'p': startdate-=NUMDAYS; goto RELOAD;
         case 'n': startdate+=NUMDAYS; goto RELOAD;
         case 'e':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }
                   viewempty=!viewempty; pos=0; redraw=1; break;
         case 's':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }

                   view_appts=!view_appts; goto RELOAD;
         case 't':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }
                   view_todos=!view_todos; goto RELOAD;
         case 'a':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }
                   view_annivs=!view_annivs; goto RELOAD;
         case 'h':
                   if(repeatchooser)
                   {
                     beep();
                     break;
                   }
                   viewheaders=!viewheaders; redraw=1; pos=0; break;
         case 'r': repeatchooser=!repeatchooser;
                   redraw=1;
                   pos=cursor=0;
                   break;
         case '2': _24hr=!_24hr;
                   redraw=1;
                   break;
         case 'f': propfont=!propfont;
                   if(propfont)
                   {
                      height=SCREEN_HEIGHT/PROP_FONT_HEIGHT;
                      curfontheight=PROP_FONT_HEIGHT;
                      setscroll(PROP_FONT_HEIGHT,height);
                   }
                   else
                   {
                      height=HEIGHT;
                      curfontheight=FIXED_FONT_HEIGHT;
                      setscroll(FIXED_FONT_HEIGHT,height);
                   }
                   redraw=1; cursor=pos=0; break;
         case KEY_UPPER_MENU:
         case KEY_LOWER_MENU:
         case 'i':
              helpscreen();
#ifndef ITALIAN
              if('d'==mygetch())
                donation_info();
#else
              mygetch();
#endif
              redraw=1;
              break;
         case KEY_BACKLIGHT: oztogglelight();
            break;
         case KEY_CALENDAR:
         case KEY_MAIN:
         case KEY_TELEPHONE:
         case KEY_MEMO:
         case KEY_MYPROGRAMS:
            ozexitto(c);
         case KEY_UPPER_ESC:
         case KEY_LOWER_ESC:
         case 'q':
            exit(0);
       }
    } while(1);
}
