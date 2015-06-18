#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <oz.h>
#include "scaldate.h"

byte monthlengths[]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static long d1,d2;

byte monthlength(byte month, unsigned year)
{
	if(month==2 && isleap(year)) return 29;
	  else return monthlengths[month-1];
}

long validate_date(char *s)
{
    int i;
    static unsigned m,d,y;
    if(strlen(s)!=10 || s[2]!='/' || s[5]!='/') return -1;
    for(i=0;i<10;i++)
    {
        if(i==2 || i==5) continue;
        if(!isdigit(s[i])) return -1;
    }
    m=atoi(s);
    d=atoi(s+3);
    y=atoi(s+6);
    if(y<1901 || y>2099 || d<1 || m<1 || m>12 || d>monthlength(m,y)) return -1;
    return ymd_to_scalar(y,m,d);
}

long get_date(byte x,byte y,char *date)
{
    static long d;
    do
    {
        if(ozeditline(x,y,date,11,238-x)<0) return -1;
    }
    while((d=validate_date(date))<0);
    return d;
}

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


byte del(byte type, char *buf)
{
    static unsigned page,offset,loc,recnum;
    register char *cluster;
    static long scaldate;
    static byte found;
    static byte delta;
    static int n;
    static byte x;
    static byte typenum;
    n=0;
    switch(type)
    {
        case TYPE_TODO:
            typenum=TYPENUM_TODO;
            delta=2+13+2;
            break;
        case TYPE_SCHEDULE:
            typenum=TYPENUM_SCHEDULE;
            delta=2+13;
            break;
        default:
            return -1;
    }
    do
    {
        found=0;
        page=offset=0;
        while(loc=ozfindnext(type,&page,&offset,&recnum))
        {

            cluster=ozloadcluster(loc);
            if(type==TYPE_TODO && ' '==cluster[2+13+2]) continue;
               /* undated or done*/
            scaldate=string_to_scalar(cluster+delta);
            if(scaldate<d1 || scaldate>d2) continue;
            ozunlink(typenum,recnum);
            found=1;
            n++;
    sprintf(buf,"Deleted %d %s entr%s!",
       n,
       type==TYPE_SCHEDULE?"schedule":"to-do",
       n==1?"y":"ies");
    x=ozputs(0,70,buf);
        }
    } while(found);
    if(n==0) x=ozputs(0,70,"No entries found in range!");
    return x;
}

main()
{
    static byte x;
    static char buf[70];
    static unsigned m,d,y;
    static byte type;
    ozcls();
    ozputs(0,0,"Deleter!");
    ozputs(0,10,"This program deletes entries by date range.");
    ozputs(0,20,"What kinds of items would you like to delete?");
    ozputs(0,30,"(S)chedule entries (except C-filesystem files)");
    ozputs(0,40,"(T)o do entries");
    switch(ozgetch())
    {
        case 's':
            type=TYPE_SCHEDULE;
            break;
        case 't':
            type=TYPE_TODO;
            break;
        default:
            return;
    }
    strcpy(buf,"01/01/1901");
    x=ozputs(0,50,"Start date for deletion? ");
    if((d1=get_date(x,50,buf))<0) return;
    scalar_to_ymd(ymd_to_scalar(ozyear(),ozmonth(),ozday())-60,&y,&m,&d);
    sprintf(buf,"%02d/%02d/%04d",m,d,y);
    x=ozputs(0,60,"End date for deletion? ");
    if((d2=get_date(x,60,buf))<0) return;
    ozputs(0,70,"Are you sure? [N]");
    if(ozgetch()!='y') return;
    x=del(type,buf);
    ozputs(x,70," Done!");
    ozgetch();
}
