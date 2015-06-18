/* clockcal.c - by billy ball (bball@tux.org)
 * version .1 - 12/12/2000
 * version .11 - 12/7/2001 (Alexander Pruss)
 *
 *               Clockcal - analog clock with calendar
 *               Copyright (C) 2000, billy ball
 *               [Modified Alexander R. Pruss]
 *                       bball@tux.org
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 * a simple analog clock w/info and calendar functions for
 * the Sharp Wizard OZ-770
 *
 * compile with:
 *
 * zxc -o -c clockcal.c
 * 
 * link and build to a .wzd file with:
 *
 * ozlink32 clockcal
 *
 * features:
 *
 * - analog clock with sweeping second hand
 * - scrolling month-to-month calendar
 * - two display modes
 *
 * LICENSE: 
 *
 * see the accompanying README and COPYING files included with this
 * program.
 *
 * This program does not use any file I/O 
 * functions in the liboz.lib as distributed by Alex Pruss.
 *
 */

#include <stdio.h>
#include <oz.h>
#include <string.h>
#include <math.h>

/* utility routines */
reverse(s)
char s[];
{
 static int i, j;
 static char c;

        for (i =0,j=strlen(s)-1; i <j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

char *itoa_array[]={ "0","1","2","3","4","5","6","7","8","9","10","11",
    "12" };

char *days_array[]={ "  0","  1","  2","  3","  4","  5","  6","  7","  8",
    "  9"," 10"," 11"," 12"," 13"," 14"," 15"," 16"," 17"," 18"," 19",
    " 20"," 21"," 22"," 23"," 24"," 25"," 26"," 27"," 28"," 29"," 30",
    " 31" }; /* faster than a call to sprintf! */

/* table construct */
struct cossintype {
       int cos;
           int sin;
};
typedef struct cossintype CossinType;    

CossinType cossin[65] = {
{160,00},{159,17},{157,33},{152,49},{146,65},{139,80},
{129,94},{119,107},{107,119},{94,129},{80,139},{65,146},
{49,152},{33,157},{17,159},{00,160},{-17,159},{-33,157},
{-49,152},{-65,146},{-80,139},{-94,129},{-107,119},{-119,107},
{-129,94},{-139,80},{-146,65},{-152,49},{-157,33},{-159,17},
{-160,00},{-159,-17},{-157,-33},{-152,-49},{-146,-65},{-139,-80},
{-129,-94},{-119,-107},{-107,-119},{-94,-129},{-80,-139},{-65,-146},
{-49,-152},{-33,-157},{-17,-159},{00,-160},{17,-159},{33,-157},
{49,-152},{65,-146},{80,-139},{94,-129},{107,-119},{119,-107},
{129,-94},{139,-80},{146,-65},{152,-49},{157,-33},{159,-17},
{160,00}
};  

int timepos = 0;

/* draw the hands of the clock */
void draw_hand(short xstart,short ystart,int timepos, int length, int width) {
 static long xwidth;
 static long ywidth;
 static short xend;
 static short yend;
 static int n;

 if (timepos>59) timepos=timepos-60;

 xwidth=(length*cossin[timepos].sin)/160;
 ywidth=(length*cossin[timepos].cos)/160;

 xend=xstart+xwidth; 
 yend=ystart-ywidth;

 ozfont(FONT_PC_NORMAL);
 for (n=1;n<13;n++) {
        /* original values: 55 (vs 57) and 35 (vs 39) */
        ozputs((int)(50+cossin[n*5].sin/5),(int)(36-cossin[n*5].cos/5),itoa_array[n]);
 }

 if (width==1) {
        _ozline(xstart,ystart,xend,yend,BLACK);
 }

 if (width==2) {
      _ozline(xstart,ystart,xend,yend,BLACK);
      _ozline(xstart-1,ystart,xend,yend,BLACK);
      _ozline(xstart+1,ystart,xend,yend,BLACK);
      _ozline(xstart,ystart-1,xend,yend,BLACK);
      _ozline(xstart,ystart+1,xend,yend,BLACK);

 }

 if (width==3) {
      _ozline(xstart,ystart,xend,yend,BLACK);
      _ozline(xstart-1,ystart,xend,yend,BLACK);
      _ozline(xstart+1,ystart,xend,yend,BLACK);
      _ozline(xstart,ystart-1,xend,yend,BLACK);
      _ozline(xstart,ystart+1,xend,yend,BLACK);
      _ozline(xstart-1,ystart-1,xend,yend,BLACK);
      _ozline(xstart+1,ystart+1,xend,yend,BLACK);
      _ozline(xstart+1,ystart-1,xend,yend,BLACK);
      _ozline(xstart-1,ystart+1,xend,yend,BLACK);
   }
} 

/* draw the clockface */
void draw_clockface()
{

        static unsigned nowhour;
        static long hour;
        static unsigned nowminute;
        static unsigned nowsec;

        nowhour=ozhour(); 
        nowminute=ozmin();
        nowsec=ozsec();   

        ozcircle(52,39,(byte)39,BLACK);
        ozcircle(52,39,(byte)37,BLACK);

        draw_hand(52,39,nowminute,24,2);    
        draw_hand(52,39,nowsec,27,1);

        if(nowhour>12) {
                nowhour = nowhour-12;
        }
        hour = (nowhour*60)+nowminute;
        hour = 60*hour/(60*12);

        draw_hand(52,39,(int)hour,18,3);  
}

/* for the info() call */
void print_wday() {
        static char *wday[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
                               "Thursday", "Friday", "Saturday", "Sunday" };
        static unsigned weekday;
        static short sign;
        static char buf[30];

        weekday=ozweekday();
        if ((sign = weekday) < 0)
                weekday = -weekday;
    /* since Alex doesn't offer the source to ozlib.lib, [YES I DO!!!]
         * and ozweekday() is not documented much,
         * the best i can determine is that if the return
         * is negative, it must be Sunday.
         */
        if (weekday < 1)
                strcpy(buf,"Today is Sunday");
        else sprintf(buf,"Today is %s",wday[weekday]);
        ozputs(110,10,buf);
}

/* calender declarations */
static int days[12]    = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static char *month[12] = {"January", "February", "March", "April", "May", "June",
                   "July", "August", "September", "October", "November", "December"};


/* alternative display enabled by 'i' */
void info(){
        static char buf[64];
        static unsigned hour;
        static unsigned minute;
        static unsigned sec;
        static unsigned day;
        static unsigned curmonth;
        static unsigned year;
/*        int sign; */

        hour=ozhour(); 
        minute=ozmin();
        sec=ozsec();
        day=ozday();
        curmonth=ozmonth();
        year=ozyear();

/*
        if ((sign = hour) < 0)
                hour = -hour;
        if ((sign = minute) < 0)
                minute = -minute;
        if ((sign = sec) < 0)
                sec = -sec;
        if ((sign = day) < 0)
                day = -day;
        if ((sign = month) < 0)
                month = -month;
        if ((sign = year) < 0)
                year = -year;
*/

        ozfont(FONT_OZ_LARGE);

        print_wday();

        sprintf(buf,"%s %d, %d", month[curmonth-1], day, year);
        ozputs(110,25,buf);

        sprintf(buf,"%d/%d/%d",curmonth,day,year);
        ozputs(110,40,buf);

        sprintf(buf,"%d:%02d:%02d",hour,minute,sec);
        ozputs(110,55,buf);

        _ozline(238,0,238,79,BLACK);
        _ozline(237,0,237,79,BLACK);
        _ozline(236,0,236,79,BLACK);
        _ozline(234,0,234,79,BLACK);
}


/*
** scalar date routines    --    public domain by Ray Gardner
** These will work over the range 1/01/01 thru 14699/12/31
*/

int isleap (unsigned yr)
{
   return yr % 400 == 0 || (yr % 4 == 0 && yr % 100 != 0);
}

unsigned months_to_days (unsigned month)
{
   return (month * 3057 - 3007) / 100;
}

long years_to_days (unsigned yr)
{
   return yr * 365L + yr / 4 - yr / 100 + yr / 400;
}

long ymd_to_scalar (unsigned yr, unsigned mo, unsigned day)
{
   static long scalar;
   scalar = day + months_to_days(mo);
   if ( mo > 2 )                         /* adjust if past February */
      scalar -= isleap(yr) ? 1 : 2;
   yr--;
   scalar += years_to_days(yr);
   return scalar;
}

void scalar_to_ymd (long scalar, unsigned *pyr, unsigned *pmo, unsigned *pday)
{
   static unsigned n;                /* compute inverse of years_to_days() */

   for ( n = (unsigned)((scalar * 400L) / 146097); years_to_days(n) < scalar;)
      n++;                          /* 146097 == years_to_days(400) */
   *pyr = n;
   n = (unsigned)(scalar - years_to_days(n-1));
   if ( n > 59 ) {                       /* adjust if past February */
      n += 2;
      if ( isleap(*pyr) )
         n -= n > 62 ? 1 : 2;
   }
   *pmo = (n * 100 + 3007) / 3057;  /* inverse of months_to_days() */
   *pday = n - months_to_days(*pmo);
}

/* print a calendar for designated month, year at x,y */
void print_cal(byte x, byte y, unsigned mo, unsigned yr)
{
      static int day, day_1, numdays;
      static char buf[64];
/*      static int sign; */
      static byte counter,zx,zy;

/*
      if ((sign = yr) < 0)
        yr = -yr;

      if ((sign = mo) < 0)
        mo = -mo;
*/

      if (100 > yr)
            yr += 1900;

      if (3000 < yr || 1800 > yr)
            exit(0);

      numdays = days[mo - 1];

      if (2 == mo && isleap(yr))
            ++numdays;

      day_1 = (int)(ymd_to_scalar(yr, mo, 1) % 7L);

      zx = x;
      zy = y;

      /* print month, year */
      sprintf(buf,"%s          %d", month[mo - 1], yr);
      ozputs(zx,zy,buf);

      /* print days of week */
      zy = zy + 10;
      ozputs(zx,zy,"Su  Mo  Tu  We  Th  Fr  Sa");
      _ozline(zx,zy+8,zx+100,zy+8,BLACK);
      zy = zy + 10;
      zx = x;
   
      /* get width of three numbers */
      counter = ozputs(0,-1,"000");

      /* calculate spacing before printing first day */
      if (day_1 < 1) {
        zx = x;
      } else
      for (day = 0; day < day_1; ++day) {
            zx =  zx + counter;
      }
     
      /* print the calendar */
      for (day = 1; day <= numdays; ++day, ++day_1, day_1 %= 7)
      {
            if (!day_1 && 1 != day)  {
                  zy = zy + 10;
                  zx = x;
            }
            ozputs(zx,zy,days_array[day]); /* CPU time saving = power saving */
            zx = zx + counter;
      }
}

/* quick help - ugly, isn't it? */
void do_help(){
        ozcls();
        ozfont(FONT_OZ_LARGE);
        ozputs(10,13,"Clockcal v.11 by bball@tux.org (modified)");
        ozputs(10,26,"Press any key to return.");
        ozputs(10,39,"Press i to use info view.");
        ozputs(10,52,"Press c to use calendar view.");
        ozputs(10,65,"Press <- or -> to scroll calendar.");
        ozgetch();
        ozfont(FONT_OZ_NORMAL);
        ozcls();
}

/* modes */
#define CALENDAR 1
#define INFO 2

main (int argc, char **argv) {
static unsigned  mo, year, c;
static byte redraw=0;
static byte oldsec;
static int status;

        year = ozyear();
        mo = ozmonth();
	ozcls();
        status = CALENDAR;

        for (;;) {
                ozsetactivepage(!ozgetdisplaypage());
                ozcls();
                oldsec=ozsec();
                draw_clockface();
                if (status == CALENDAR)
                        print_cal(110,0,mo,year);
                if (status == INFO)
                        info();
                ozdisplayactivepage();
                if (ozkeyhit()) {
                        switch(c=ozgetch()) {
                                case KEY_LOWER_MENU:
                                case KEY_UPPER_MENU:
                                case 'h': do_help(); break;
                                case KEY_LOWER_ESC:
                                case KEY_UPPER_ESC:
                                case KEY_MYPROGRAMS:
                                case 'q': exit(0); break;
                                case KEY_CALENDAR:
                                case KEY_POWER:
                                case KEY_MAIN:
                                case KEY_TELEPHONE:
                                case KEY_MEMO:
                                    ozexitto(c);
                                case KEY_BACKLIGHT:
                                    oztogglelight();
                                    break;
                                case KEY_RIGHT:
                                        if (status == CALENDAR) {
						/* advance year if Dec. */
                                                if (mo == 12) {
                                                        mo = 1;
                                                        ++year;
                                                } else ++mo;
                                        }
                                        redraw=1;
                                        break;
                                case KEY_LEFT:
                                        if (status == CALENDAR) {
						/* decr year if Jan. */
                                                if (mo == 1) {
                                                        mo = 12;
                                                        --year;
                                                } else --mo;
                                        }
                                        redraw=1;
                                        break;
                                case 'c': status = CALENDAR;
                                        redraw=1;
                                        break;
                                case 'i': status = INFO;
                                        redraw=1;
                                        break;
                                default: break;
                        }
                }
                while(!redraw && (ozsec()==oldsec) && !ozkeyhit()) ozintwait();
                  /* this is for power saving: ozintwait() makes the CPU
                     idle -- but must be compiled with ozlink32 */
	}
}

/* end */
