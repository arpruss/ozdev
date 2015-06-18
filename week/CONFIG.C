#include <ctype.h>
#include <string.h>
#include <oz.h>

enum option_items { VIEWEMPTY, VIEWHEADERS, VIEWTODOS, VIEWANNIVS,
    VIEWAPPTS, ALIGNAT, BEEPS, VIEWMAINHEADER, BIGFONT, HELP,
    REPEATCHOOSER, OPT24HR, FORCEAUTORUN };

extern byte _24hr;
extern char alignat;
extern byte viewempty;
extern char viewheaders;
extern byte view_todos;
extern byte view_annivs;
extern byte view_appts;
extern byte mainheader;
extern byte silent;
extern byte propfont;
extern byte repeatchooser;
extern unsigned help;

#define OPTLEN 15
struct option
{
    char *keyword;
    int item;
}
options[]=
{
    { "24hour", OPT24HR },
    { "emptydays", VIEWEMPTY },
    { "headers", VIEWHEADERS },
    { "dayheaders", VIEWHEADERS },
    { "todos", VIEWTODOS },
    { "anniversaries", VIEWANNIVS },
    { "schedule", VIEWAPPTS },
    { "alignat", ALIGNAT },
    { "weekheader", VIEWMAINHEADER },
    { "beeps", BEEPS },
    { "bigfont", BIGFONT },
    { "repeateditor", REPEATCHOOSER },
    { "helpmessage", HELP },
    { "forceautorun", FORCEAUTORUN }
};

void handleoption(int item, char *o)
{
    static byte slot;
    switch(item)
    {
        case OPT24HR:
          _24hr=(*o!='0');
          break;
        case FORCEAUTORUN:
          if(*o!='0' && (slot=ozgetmyslot())) ozsetautorun(slot);
          break;
        case HELP:
          help=atoi(o);
          break;
        case BEEPS:
          silent=(*o=='0');
          break;
        case BIGFONT:
          propfont=(*o!='0');
          break;
        case REPEATCHOOSER:
          repeatchooser=(*o!='0');
          break;
        case VIEWMAINHEADER:
          mainheader=(*o!='0');
          break;
        case VIEWEMPTY:
          viewempty=(*o!='0');
          break;
        case VIEWHEADERS:
          viewheaders=(*o!='0');
          break;
        case VIEWTODOS:
          view_todos=(*o!='0');
          break;
        case VIEWANNIVS:
          view_annivs=(*o!='0');
          break;
        case VIEWAPPTS:
          view_appts=(*o!='0');
          break;
        case ALIGNAT:
          alignat=atoi(o);
          break;
     }
}

void configure(void)
{
    static int h;
    static int c;
    char key[OPTLEN+1];
    char theoption[80];
    static int keypos,optpos,i;
    h=ozopenmemo("zz:viewweek.config");
    if(h==-1)
    {
#ifndef ITALIAN
        termputs("No 'zz:viewweek.config' file.\r\n");
#else /* ITALIAN */
        termputs("'zz:viewweek.config' inesistente.\r\n");
#endif /* ITALIAN */
        return;
    }
    for(i=0;i<43;i++) ozreadfilebyte();

    while(-1!=(c=ozreadfilebyte()) && c)
    {
        if(c=='#')
        {
            while(-1!=(c=ozreadfilebyte()) && c!='\r' && c!='\xff'
             && c);
            if(c==-1 || c=='\xff' ||!c) break;  else continue;
        }
        if(c=='\r' || isspace(c)) continue;
        keypos=0;
        do
          key[keypos++]=c;
        while((isalpha(c=ozreadfilebyte()) || isdigit(c)) && keypos<OPTLEN);
        key[keypos]=0;
        termputs(key);
        if(c==-1 || c=='\xff' || !c) break;
        if(keypos==OPTLEN || !isspace(c))
        {
            while(-1!=(c=ozreadfilebyte()) && c!='\r' && c!='\xff' && c);
            if(c==-1 || c=='\xff' || !c) break; else continue;
        }
        while(-1!=(c=ozreadfilebyte()) && c && isspace(c));
        if(-1==c || !c) break;
        optpos=0;
        do
          theoption[optpos++]=c;
        while((c=ozreadfilebyte())!=-1 && c!='\r' && c!='\xff' && c);
        theoption[optpos]=0;
        termputch(' ');
        termputs(theoption);
        termputs("\r\n");
        for(i=0;i<sizeof(options)/sizeof(*options);i++)
         if(!strcmp(options[i].keyword,key))
         {
            handleoption(options[i].item,theoption);
            break;
         }
        if(i==sizeof(options)/sizeof(*options))
        {
#ifndef ITALIAN
            termputs("Unknown option ");
#else /* ITALIAN */
            termputs("Opzione sconosciuta ");
#endif /* ITALIAN */
            termputs(key);
            termputs(".\n\r");
        }
    }
    ozclosefile();
}





