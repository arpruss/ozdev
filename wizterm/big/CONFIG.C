#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <oz.h>
#include "ozread.h"

enum option_items { BAUD, STOPBITS, DATABITS, PARITY, SMOOTHSCROLL,
    HANDSHAKING, AUTOLF, KEY, LINES, COLUMNS, LOCALECHO, VISBEEP,
    CLICK };

#define SOFTKEYAREA 2048
char softkeyarea[SOFTKEYAREA];
char *softkeyptr=softkeyarea;

extern char *softkeys_2nd_alpha[26];
extern char *softkeys_cat_alpha[26];
extern char *softkeys_2nd_num[10];
extern char *softkeys_cat_num[10];
extern byte smoothscroll;
extern byte LNM;
extern byte echo;
extern byte handshaking;
extern byte visualbeep;

#define OPTLEN 14
struct option
{
    char *keyword;
    int item;
}
options[]=
{
    { "localecho", LOCALECHO },
    { "baud", BAUD },
    { "stopbits", STOPBITS },
    { "databits", DATABITS },
    { "parity", PARITY },
    { "smoothscroll", SMOOTHSCROLL },
    { "handshaking", HANDSHAKING },
    { "autolf", AUTOLF },
    { "key", KEY },
    { "lines", LINES },
    { "columns", COLUMNS },
    { "visualbeep", VISBEEP },
    { "keyclick", CLICK }
};

char *skipwhite(char *s)
{
    while(*s && isspace(*s)) s++;
    return s;
}

unsigned hexnibble(char c)
{
    if(isdigit(c)) return c-'0';
      else return tolower(c)-'a'+10;
}

void convertstring(char *s)
{
    register char *t;
    static unsigned value;
    if(!*s) return;
    t=s+strlen(s)-1;
    while(t>=s && isspace(*t))
    {
        *t=0;
        t--;
    }
    if(t<s) return;
    if(*s=='"' && *t=='"') *t=0;
    t=s;
    while(*s)
    {
        if(*s=='\\')
        {
            s++;
            switch(*s)
            {
                case '\0': *t=0; return;
                case '\\': *t++='\\'; break;
                case 'N':
                case 'n': *t++='\n'; break;
                case 'R':
                case 'r': *t++='\r'; break;
                case 'T':
                case 't': *t++='\t'; break;
                case '"': *t++='"'; break;
                case 'X':
                case 'x': *t++=hexnibble(s[1])*16+hexnibble(s[2]);
                          s+=2; break;
                default:
                  if(isdigit(*s))
                  {
                     value=*s-'0';
                     if(isdigit(s[1]))
                     {
                        s++;
                        value=value*10+*s-'0';
                        if(isdigit(s[1]))
                        {
                          s++;
                          value=value*10+*s-'0';
                        }
                     }
                     *t++=value;
                  }
                  else *t++=*s;
                  break;
            }
        }
        else *t++=*s;
        s++;
    }
    *t=0;
}


char *processkey(char *s)
{
    static int l;
    register char *oldskp;
    convertstring(s);
    if(softkeyptr+(l=strlen(s))+1>softkeyarea+SOFTKEYAREA)
    {
        termputs("Out of softkey memory.\r\n");
        return NULL;
    }
    strcpy(oldskp=softkeyptr,s);
    softkeyptr+=l+1;
    return oldskp;
}

void handleoption(int item, char *o)
{
    switch(item)
    {
        case COLUMNS:
            if(atoi(o)>=30) setcols(30); else setcols(29);
            break;
        case DATABITS:
          switch(atoi(o))
          {
            case 5: ozdatabits(DATABITS5); break;
            case 6: ozdatabits(DATABITS6); break;
            case 7: ozdatabits(DATABITS7); break;
            case 8: ozdatabits(DATABITS8); break;
          }
          break;
        case BAUD:
          if(!strncmp(o,"38400",5))
             ozsetbaud(BAUD38400);
          else switch(atoi(o))
          {
            case 300: ozsetbaud(BAUD300); break;
            case 1200: ozsetbaud(BAUD1200); break;
            case 2400: ozsetbaud(BAUD2400); break;
            case 4800: ozsetbaud(BAUD4800); break;
            case 9600: ozsetbaud(BAUD9600); break;
            case 19200: ozsetbaud(BAUD19200); break;
            case 25600: ozsetbaud(BAUD25600); break;
            case 30720: ozsetbaud(BAUD30720); break;
          }
          break;
         case STOPBITS:
           if(!strncmp(o,"1.5",3))
             ozstopbits(ONE_POINT_FIVE_STOP_BITS);
           else switch(atoi(o))
           {
            case 1: ozstopbits(ONE_STOP_BIT); break;
            case 2: ozstopbits(TWO_STOP_BITS); break;
           }
           break;
         case PARITY:
           if(!strncmp(o,"odd",3))
             ozparity(ODD_PARITY);
           else if(!strncmp(o,"even",4))
             ozparity(EVEN_PARITY);
           else if(!strncmp(o,"none",4))
             ozparity(NO_PARITY);
           else if(!strncmp(o,"high",4))
             ozparity(HIGH_PARITY);
           else if(!strncmp(o,"low",3))
             ozparity(LOW_PARITY);
           break;
         case SMOOTHSCROLL:
           smoothscroll=(*o!='0');
           break;
         case HANDSHAKING:
           handshaking=(*o!='0');
           break;
         case LOCALECHO:
           echo=(*o!='0');
           break;
         case VISBEEP:
           visualbeep=(*o!='0');
           break;
         case AUTOLF:
           LNM=(*o!='0');
           break;
         case CLICK:
           ozclick(*o!='0');
           break;
         case LINES:
           if(atoi(o)>14) setlines16(); else setlines13();
           break;
         case KEY:
           if(!strncmp(o,"category-",9))
           {
            if(isalpha(o[9]))
              softkeys_cat_alpha[o[9]-'a']=processkey(skipwhite(o+10));
            else if(isdigit(o[9]))
              softkeys_cat_num[o[9]-'0']=processkey(skipwhite(o+10));
            else
            {
                termputs("Unknown key: category-");
                termputch(o[9]);
                termputs("\r\n");
            }
           }
           else if(!strncmp(o,"2nd-",4))
           {
            if(isalpha(o[4]))
              softkeys_2nd_alpha[o[4]-'a']=processkey(skipwhite(o+5));
            else if(isdigit(o[4]))
              softkeys_cat_num[o[4]-'0']=processkey(skipwhite(o+5));
            else
            {
                termputs("Unknown key: category-");
                termputch(o[4]);
                termputs("\r\n");
            }
           }
           else
           {
            termputs("Cannot handle key definition ");
            termputs(o);
            termputs("\r\n");
           }
           break;
     }
}

void configure(void)
{
    static int h;
    static int c;
    static char key[OPTLEN+1];
    static char theoption[350];
    static int keypos,optpos,i;
    h=ozopenread("zz:wizterm.config");
#if 0
    {
        extern unsigned saved_bc, saved_hl;
        termputs("saved_bc=");
        termputs(utoa(saved_bc));
        termputs(" saved_hl=");
        termputs(utoa(saved_hl));
        termputs("\r\n");
    }
#endif
    if(h==-1)
    {
        termputs("No 'zz:wizterm.config' file.\r\n");
        return;
    }
    for(i=0;i<43;i++) ozgetc(h);
/*    while(-1!=(c=ozgetc(h))) termputch(c=='\r'?'*':c);
    return; */

    while(-1!=(c=ozgetc(h)))
    {
        if(c=='#')
        {
            while(-1!=(c=ozgetc(h)) && c!='\r' && c!='\xff' && c);
            if(c==-1 || c=='\xff' || !c) break;  else continue;
        }
        if(c=='\r' || isspace(c)) continue;
        keypos=0;
        do
          key[keypos++]=c;
        while(isalpha(c=ozgetc(h)) && keypos<OPTLEN);
        key[keypos]=0;
        if(c==-1 || c=='\xff' || !c) break;
        if(keypos==OPTLEN || !isspace(c))
        {
            while(-1!=(c=ozgetc(h)) && c!='\r' && c!='\xff');
            if(c==-1 || c=='\xff') break; else continue;
        }
        while(-1!=(c=ozgetc(h)) && isspace(c));
        if(-1==c) break;
        optpos=0;
        do
          theoption[optpos++]=c;
        while((c=ozgetc(h))!=-1 && c!='\r' && c!='\xff');
        theoption[optpos]=0;
        for(i=0;i<sizeof(options)/sizeof(*options);i++)
         if(!strcmp(options[i].keyword,key))
         {
/*
            if(options[i].item!=KEY)
            {
              termputs(key);
              termputch(' ');
              termputs(theoption);
              termputs("\r\n");
            }
*/
            handleoption(options[i].item,theoption);
            break;
         }
        if(i==sizeof(options)/sizeof(*options))
        {
            termputs("Unknown option ");
            termputs(key);
            termputs(".\n\r");
        }
    }
    ozclose(h);
}

