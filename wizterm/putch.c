#include <oz.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "term.h"

#define WIDTH 60 /* 60 is OK if bleeding isn't a problem */

int ttyprint(byte c);
void ttyscroll(void);

#define MAX_ESC_SEQ 32
byte height=13;
byte width=WIDTH;
byte charheight=6;
byte visualbeep;
extern char currow;
extern char curcol;
extern byte smoothscroll;
char region_top=0;
char region_bottom=13-1;  /* default height */
byte LNM=0;
byte esc_seq=0;
static char the_esc_seq[MAX_ESC_SEQ+1];
static unsigned char esc_seq_ptr;

static char tabs[WIDTH+1];

static int arg1,arg2;

extern char reverse,underline;

void setmode(char *m, byte state)
{
    while(m!=NULL && *m)
    {
        switch(atoi(m))
        {
            case 20: LNM=state;
            case 4: smoothscroll=state;
        }
        m=strchr(*m==';'?m+1:m,';');
        m=(m==NULL)?(char*)NULL:m+1;
    }
}

char *utoa(unsigned u)
{
    static char buffer[10];
    register char *b;
    b=buffer+9;
    *b=0;
    while(u || b==buffer+9)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    return b;
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

void get2args(void)
{
    register char *s;
    if(NULL==(s=strchr(the_esc_seq+1,';')))
    {
        arg1=atoi(the_esc_seq+1);
        arg2=0;
        return;
    }
    *s++=0;
    arg1=atoi(the_esc_seq+1);
    arg2=atoi(s);
}


static void newline(void)
{
    xorcursor();
    currow++;
    if(currow>region_bottom)
    {
        currow--;
        ttyscroll();
    }
    xorcursor();
}

static void revnewline(void)
{
    xorcursor();
    currow--;
    if(currow<region_top)
    {
        currow++;
        ttyscrolldown();
    }
    xorcursor();
}

/*
lp(char *s)
{
    while(*s) ttyprint(*s++);
}
*/

void scrolling_region(char row1, char row2)
{
    extern unsigned scrolltop, scrolllen, scrollbotline;
    if(row1==0) row1=0; else row1--;
    if(row2==0) row2=height-1; else row2--;
    if(row1<0) row1=0;
    if(row1>=height) row1=height-1;
    if(row2<row1) row2=row1;
    if(row2>=height) row2=height-1;
    region_top=row1;
    region_bottom=row2;
    scrolltop=(row1)*(30*charheight)+0xa000;
    scrolllen=(row2-row1)*(30*charheight);
    scrollbotline=0xa000+(row2)*(30*charheight);
}

void reset_term(void)
{
    static byte i;
    ozcls();
    curcol=currow=0;
    scrolling_region(1,height);
    LNM=smoothscroll=reverse=underline=0;
    esc_seq=0;
    xorcursor();
    for(i=1;i<WIDTH;i++)
        tabs[i]=(i%8)?'.':'T';
    tabs[0]='.';
    tabs[WIDTH]=0;
}

void crlf(void)
{
    xorcursor();
    curcol=0;
    currow++;
    if(currow>region_bottom)
    {
        currow--;
        ttyscroll();
    }
    xorcursor();
}

void clearregion(byte x1,byte y1,byte x2,byte y2)
{
    static byte len;
    static unsigned from;
    static byte numttylines;
    static byte mask1;
    static byte mask2;
    static byte thelen;
    static byte i;
    if(x1>x2 || y1>y2) return;
    if(x1>=WIDTH || y1>=height) return;
    if(x2>=WIDTH) x2=WIDTH-1;
    if(y2>=height) y2=height-1;
    from=charheight*30*(unsigned)y1+(x1>>1);
    numttylines=y2-y1+1;
    len=x2-x1+1;
    if(len==1)
    {
        mask1=(x1 & 1)?0x0F:0xF0;
        thelen=1;
    }
    else if(len==2)
    {
        if(x1 & 1)
        {
            mask1=0x0F;
            thelen=2;
            mask2=0xF0;
        }
        else
        {
            mask1=0;
            thelen=1;
        }
    }
    else
    {
        if(x1 & 1)
        {
            mask1=0x0F;
            thelen=1+(len>>1);
            mask2=(x2 & 1)?0:0xF0;
        }
        else
        {
            mask1=0;
            thelen=(len+1)>>1;
            mask2=(x2 & 1)?0:0xF0;
        }
    }
    for(i=0;i<numttylines;i++)
    {
        ttyclearregion(from,thelen,mask1,mask2,charheight);
        from+=30*charheight;
    }
}

static byte saved_row, saved_col, saved_reverse, saved_underline;

void save_cursor(void)
{
    saved_row=currow;
    saved_col=curcol;
    saved_reverse=reverse;
    saved_underline=underline;
}

void restore_cursor(void)
{
    xorcursor();
    currow=saved_row;
    curcol=saved_col;
    reverse=saved_reverse;
    underline=saved_underline;
    xorcursor();
}

static byte saved_row2, saved_col2, saved_reverse2, saved_underline2;

void save_cursor2(void)
{
    saved_row2=currow;
    saved_col2=curcol;
    saved_reverse2=reverse;
    saved_underline2=underline;
}

void restore_cursor2(void)
{
    xorcursor();
    currow=saved_row2;
    curcol=saved_col2;
    reverse=saved_reverse2;
    underline=saved_underline2;
    xorcursor();
}


void cursor_down(char n)
{
    xorcursor();
    currow+=n;
    if(currow>region_bottom) currow=region_bottom;
    xorcursor();
}

void cursor_up(char n)
{
    xorcursor();
    currow-=n;
    if(currow<region_top) currow=region_top;
    xorcursor();
}

void cursor_left(char n)
{
    xorcursor();
    curcol-=n;
    if(curcol<0) curcol=0;
    xorcursor();
}

void cursor_right(char n)
{
    xorcursor();
    curcol+=n;
    if(curcol>=width) curcol=width-1;
    xorcursor();
}

void cursor_move(int row,int col)
{
    xorcursor();
    curcol=col-1;
    if(curcol>=width) curcol=width-1;
     else if(curcol<0) curcol=0;
    currow=row-1;
    if(currow>=height) currow=height-1;
     else if(currow<0) currow=0;
    xorcursor();
}

void attributes(char *s)
{
    while(s!=NULL)
    {
        if(!isdigit(s[1])) underline=reverse=0;
        else
        switch(atoi(s+1))
        {
            case 0: underline=reverse=0; break;
            case 1: if(height==13) underline=1; break;  /* should be BOLD */
            case 4: if(height==13) underline=1; break;
            case 7: reverse=1; break;
        }
        s=strchr(s+1,';');
    }
}

void termputch(byte c)
{
#ifdef VT52
    static int cursor_address=0;
    static char caddress_row;
#endif
    if(!esc_seq
#ifdef VT52
     && !cursor_address
#endif
    )
    switch(c)
    {
        case 7: if(visualbeep)
                {
                    static unsigned i;
                    oztogglelight();
                    for(i=0;i<1000;i++);
                    oztogglelight();
                }
                else beep();
            return;
        case 8: xorcursor();
            if(curcol>0) curcol--;
             else curcol=0;
            xorcursor();
            return;
        case '\t': {
                     char *t;
                     if(curcol==width ||
                       NULL==(t=strchr(tabs+curcol+1,'T')))
                     {
                        crlf();
                     }
                     else
                     {
                        xorcursor();
                        curcol=t-tabs;
                        xorcursor();
                     }
                     return;
                   }
        case 11:
        case 12:
        case '\n': newline(); return;
        case '\r':
            if(LNM) crlf();
            else
            {
                xorcursor(); curcol=0; xorcursor();
            }
            return;
        case 27: esc_seq=1; esc_seq_ptr=0; return;
        case 14: /* ignore G1 charset */
        case 15: /* ignore G0 charset */
        case 17: /* ignore XON */
        case 19: /* ignore XOFF */
        case 127: /* fill character, ignore */
          return;
        default:
DOPUTCH:
            ttyprint(c);
            return;
    }
#ifdef VT52
    if(cursor_address==1)
    {
        caddress_row=c-30;
        cursor_address=2;
        return;
    }
    if(cursor_address==2)
    {
        cursor_move(caddress_row,c-30);
        cursor_address=0;
        return;
    }
#endif
    if(!esc_seq_ptr)
        switch(c)
        {
            case '[':
            case '(':
            case ')':
               the_esc_seq[0]=c;
               esc_seq_ptr=1;
               return;
            case 'H':
               tabs[curcol<width?curcol:0]='T';
               esc_seq=0;
               return;
            case 'c':
               reset_term();
               esc_seq=0;
               return;
            case '7':
               save_cursor();
               esc_seq=0;
               return;
            case '8':
               restore_cursor();
               esc_seq=0;
               return;
            case 'E':
               newline();
               xorcursor();
               curcol=0;
               xorcursor();
               esc_seq=0;
               return;
            case 'D':
               newline();
               esc_seq=0;
               return;
            case 'M':
               revnewline();
               esc_seq=0;
               return;
#ifdef VT52
            case 'A':
               cursor_up(1);
               esc_seq=0;
               return;
            case 'B':
               cursor_down(1);
               esc_seq=0;
               return;
            case 'C':
               cursor_right(1);
               esc_seq=0;
               return;
            case 'D':
               cursor_left(1);
               esc_seq=0;
               return;
            case 'H':
               cursor_move(1,1);
               esc_seq=0;
               return;
            case 'Y':
               cursor_address=1;
               esc_seq=0;
               return;
#endif
            default:
               esc_seq=0;
               return;
        }
    if(!isalpha(c))
    {
        if(c==24 || c==26)  /* CAN */
        {
            esc_seq=0;
            c='*';
            goto DOPUTCH;
        }
        if(esc_seq_ptr>=MAX_ESC_SEQ-1)
        {
            esc_seq=0;
            c='*';
            goto DOPUTCH;
        }
        the_esc_seq[esc_seq_ptr++]=c;
        return;
    }
    if(the_esc_seq[0]=='(' || the_esc_seq[0]==')')
    {
        /* font switch: IGNORE */
        esc_seq=0;
        return;
    }
    the_esc_seq[esc_seq_ptr]=0;
    switch(c)
    {
        case 'A': cursor_up(esc_seq_ptr==1?1:atoi(the_esc_seq+1));
                  break;
        case 'B': cursor_down(esc_seq_ptr==1?1:atoi(the_esc_seq+1));
                  break;
        case 'C': cursor_right(esc_seq_ptr==1?1:atoi(the_esc_seq+1));
                  break;
        case 'D': cursor_left(esc_seq_ptr==1?1:atoi(the_esc_seq+1));
                  break;
        case 'c': serputs("\x1B[?1;0c"); /* device attributes */
                  break;
        case 'g': if(esc_seq_ptr==1 || the_esc_seq[1]=='0')
                    tabs[curcol<width?curcol:0]='.';
                  else if(the_esc_seq[1]=='3')
                  {
                    int i;
                    for(i=0;i<width;i++)
                     tabs[i]='.';
                  }
                  break;
        case 'n': if(esc_seq_ptr>1)
                  {
                    if(the_esc_seq[1]=='5') serputs("\x1B[0n");
                    else if(the_esc_seq[1]=='6')
                    {
                        ozserialout(0x1B);
                        ozserialout('[');
                        serputs(utoa(currow+1));
                        ozserialout(';');
                        serputs(utoa(curcol+1));
                        ozserialout('R');
                    }
                  }
                  break;

        case 'm': attributes(the_esc_seq); break;

        case 'K':
                  if(esc_seq_ptr==1)
                  {
                   xorcursor();
                   clearregion(curcol,currow,WIDTH-1,currow);
                     /* Esc [ K : clear to EOL */
                   xorcursor();
                  }
                  else if(the_esc_seq[1]=='1')
                  {
                   xorcursor();
                   clearregion(0,currow,curcol,currow);
                     /* Esc [ 1 K : clear from BOL */
                   xorcursor();
                  }
                  else if(the_esc_seq[1]=='2')
                  {
                   xorcursor();
                   clearregion(0,currow,WIDTH-1,currow);
                     /* Esc [ 2 K : clear whole line */
                   xorcursor();
                  }
                  break;
        case 'J': if(esc_seq_ptr==1)
                  {
                    xorcursor();
                    clearregion(curcol,currow,WIDTH-1,currow);
                    clearregion(0,currow+1,WIDTH-1,height-1);
                    xorcursor();
                      /* Esc [ J: clear to EOS */
                  }
                  else if(the_esc_seq[1]=='1')
                  {
                    xorcursor();
                    clearregion(0,0,WIDTH-1,currow-1);
                    clearregion(0,currow,curcol,currow);
                    xorcursor();
                      /* Esc [ 1 J: clear from BOS */
                  }
                  else if(the_esc_seq[1]=='2')
                  {
                    clearregion(0,0,WIDTH-1,height-1);
                      /* Esc [ 2 J: clear whole screen */
                    currow=curcol=0; /* is this needed? */
                    xorcursor();
                  }
                  break;
        case 'l':
                  setmode(the_esc_seq+1,0);
                  break;
        case 'h':
                  setmode(the_esc_seq+1,1);
                  break;
        case '1':
        case 'r':
                  get2args();
                  scrolling_region(arg1,arg2);
                  break;
        case 'H':
        case 'f':
                  get2args();
                  if(arg1==0) arg1=1;
                  if(arg2==0) arg2=1;
                  cursor_move(arg1,arg2);
                  break;

        default:
            esc_seq=0;
            goto DOPUTCH;
    }
    esc_seq=0;
}

