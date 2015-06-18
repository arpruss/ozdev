#include <stdio.h>
#include <ctype.h>
#include <oz.h>
#include "term.h"

#define SHORTCUT
#define MODEL32K

extern byte _ozrxhandshaking;
extern byte _ozrxxoff;
byte handshaking=0;

char *softkeys_2nd_alpha[26];
char *softkeys_cat_alpha[26];
char *softkeys_2nd_num[10];
char *softkeys_cat_num[10];

extern byte _ozserbufget,_ozserbufput;

void sendbreak(unsigned len)
{
    unsigned start_timer=_oz64hz_word;
    ozsetlcr(BREAKSTATE | ozgetlcr());
    while((unsigned)(_oz64hz_word-start_timer)<len) ozintwait();
    ozsetlcr((~BREAKSTATE) & ozgetlcr());
}

void setupsound(void)
{
    ozportout(0x19,1);
    ozportout(0x17,10);
    ozportout(0x18,0);
    ozportout(0x16,2);
}

void serputs_echo(char *s)
{
    while(*s)
    {
        ozserialout_echo(*s);
        s++;
    }
}

void serputs(char *s)
{
    while(*s)
    {
        ozserialout(*s);
        s++;
    }
}

void termputs(char *s)
{
    while(*s)
    {
        termputch(*s);
        s++;
    }
}

void lowputs(char *s)
{
    extern byte currow,curcol;
    while(*s)
    {
        if(*s=='\n')
        {
            xorcursor();
            currow++;
            curcol=0;
            xorcursor();
        }
        else ttyprint(*s);
        s++;
    }
}

void showbaud(void)
{
    switch(ozgetbaud())
    {
        case BAUD300: lowputs("300"); break;
        case BAUD1200: lowputs("1200"); break;
        case BAUD2400: lowputs("2400"); break;
        case BAUD4800: lowputs("4800"); break;
        case BAUD9600: lowputs("9600"); break;
        case BAUD19200: lowputs("19200"); break;
        case BAUD25600: lowputs("25600"); break;
        case BAUD30720: lowputs("30720"); break;
        case BAUD38400: lowputs("38400"); break;
    }
}

void domenu(void)
{
    extern byte currow,curcol,reverse,height,width;
    save_cursor2();
    xorcursor();
    ozcopypage(1,0);
    ozcls();
    currow=curcol=reverse=0;
    xorcursor();
    lowputs("Current baud rate: ");
    showbaud();
    lowputs("\nHandshaking: ");
    lowputs(_ozrxhandshaking?"xon/xoff":"none");
    lowputs("\nLocal echo: ");
    lowputs(echo?"on":"off");
    lowputs("\nDisplay columns: ");
    lowputs(utoa(width));
    lowputs("\n"
"300,1200,2400,4800,9600,\n"
"(a)19200 or (b)38400 ?\n"
"o: off: xon/xoff handshake\n"
"x: on: xon/xoff handshake\n"
"l: Toggle local echo\n"
"w: Toggle display width\n");
    switch(ozgetch())
    {
        case 'l': echo=!echo; break;
        case '3': ozsetbaud(BAUD300); break;
        case '1': ozsetbaud(BAUD1200); break;
        case '2': ozsetbaud(BAUD2400); break;
        case '4': ozsetbaud(BAUD4800); break;
        case '9': ozsetbaud(BAUD9600); break;
        case 'a': ozsetbaud(BAUD19200); break;
        case 'b': ozsetbaud(BAUD38400); break;
        case 'o': if(!_ozrxhandshaking) break;
                  _ozrxhandshaking=0;
                  if(_ozrxxoff)
                  {
                    ozserialout(XON);
                    _ozrxxoff=0;
                  }
                  break;
        case 'x': _ozrxhandshaking=1; break;
        case 'w': setcols(width==29?30:29);
                  break;

    }
    ozcopypage(0,1);
    xorcursor();
    restore_cursor2();
}

void do_help()
{
    extern byte currow,curcol,reverse;
    save_cursor2();
    xorcursor();
    ozcopypage(1,0);
    ozcls();
    currow=curcol=reverse=0;
    xorcursor();
    reverse=0;
    /*       12345678901234567890123456789 */
    lowputs("MY PROGRAMS:  quit\n"
            "upper-MENU:   set options\n"
            "lower-MENU:   help screen\n"
            "2ND + v:      paste\n"
            "2ND + key:    purple symbols\n"
            "NEW + key:    CTRL keys\n"
            "CATEGORY  +   opqr" "w190,.\n"
            "      yields: []~\\""`|{}<>\n"
            "CATEGORY + b: short break\n"
            "CATEGORY + l: long break\n");
    ozgetch();
    ozcopypage(0,1);
    xorcursor();
    restore_cursor2();
}

void get_key(void)
{
    extern char LNM;
    static unsigned int c;
    c=getch();
    switch(c)
    {
        case KEY_BACKLIGHT:
           oztogglelight();
           return;
        case KEY_LOWER_MENU:
           do_help();
           return;
        case KEY_UPPER_MENU:
           domenu();
           return;
        case KEY_MYPROGRAMS:
        case KEY_MAIN:
        case KEY_TELEPHONE:
        case KEY_MEMO:
        case KEY_CALENDAR:
           ozexitto(c);
           return;
        case KEY_UPPER_ENTER:
        case '\r':
           ozserialout_echo('\r');
           if(LNM) ozserialout('\n');
           break;
        case KEY_UP:
           serputs_echo("\x1B[A");
           break;
        case KEY_DOWN:
           serputs_echo("\x1B[B");
           break;
        case KEY_RIGHT:
           serputs_echo("\x1B[C");
           break;
        case KEY_LEFT:
           serputs_echo("\x1B[D");
           break;
        case KEY_UPPER_ESC:
           ozserialout_echo('\x1B');
           break;
        default:
           if(c&MASKCATEGORY) switch(c&0xff)
           {
/*                 case 'a': serputs_echo("atl1&d0&k4\r"); handshaking=1;
                           return;
 */
                 case 'b': sendbreak(15); return;
                 case 'l': sendbreak(224); return;
/*
                 case 'd': serputs_echo("atdt2977488\r");
                           return;
                 case 's': serputs_echo("TERM=vt100;export TERM;stty cols 60 rows 13\r");
                           return;
*/
                 default:
                   if(isalpha(c&0xff) &&
                     NULL!=softkeys_cat_alpha[(c&0xff)-'a'])
                     serputs_echo(softkeys_cat_alpha[(c&0xff)-'a']);
                   else if(isdigit(c&0xff) &&
                     NULL!=softkeys_cat_num[(c&0xff)-'0'])
                     serputs_echo(softkeys_cat_alpha[(c&0xff)-'0']);
                   else ozserialout_echo(c);
                   return;
           }
           else if(c&MASK2ND)
           {
                   if((c&0xf0ff)=='v') paste_to_serial();
/*                   else if((c&0xf0ff)=='c') copy_the_screen(); */
                   else if(isalpha(c&0xf0ff) &&
                     NULL!=softkeys_2nd_alpha[(c&0xf0ff)-'a'])
                     serputs_echo(softkeys_2nd_alpha[(c&0xf0ff)-'a']);
                   else if(isdigit(c&0xf0ff) &&
                     NULL!=softkeys_2nd_num[(c&0xf0ff)-'0'])
                     serputs_echo(softkeys_2nd_alpha[(c&0xf0ff)-'0']);
                   else ozserialout_echo(c);
                   return;
           }
           ozserialout_echo(c);
     }
}

main()
{
    static int c;
    static char saved_lcr;
    static unsigned saved_baud;
    static byte port7;
    extern byte reverse;
/*
    unsigned char maxbuf=0;
    static char debugbuf[60];
 */
    saved_lcr=ozgetlcr();
    saved_baud=ozgetbaud();
    ozstopbits(ONE_STOP_BIT);
    ozparity(NO_PARITY);
    ozdatabits(DATABITS8);
    ozsetbaud(BAUD9600);
    ozportout(0x41,1);
    setupsound();
    reset_term();
    port7=ozportin(7);
    ozportout(7,0xff-(4+32));
    ozkeymapcrtolf(0);
#ifndef MODEL32K
    if(ozsetisr(_ozcustomisr))
    {
        reverse=1;
        termputs("Bad OS version.\n\rPress any key to exit.\n\r");
        ozgetch();
        return;
    }
#endif
    configure();
    _ozrxxoff=0;
    if(handshaking)
        _ozrxhandshaking=1;
    else
        _ozrxhandshaking=0;
    termputs("BigTerm 1.0.15 \r\n");
    termputs("By Alexander R. Pruss.\r\n"
             "Press lower MENU key for help\r\n"
             "and MY PROGRAMS to exit.\r\n\r\n");
    while(1)
    {
        ozintwait();
/*        if((unsigned char)(_ozserbufput-_ozserbufget)>maxbuf)
             maxbuf=_ozserbufput-_ozserbufget; */
#if 0
        if(handshaking)
        {
            if((unsigned char)(_ozserbufput-_ozserbufget)>200)
            {
                ozserialout(XOFF);
                xoff=1;
            }
            else
            if(xoff && (unsigned char)(_ozserbufput-_ozserbufget)<150)
            {
                ozserialout(XON);
                xoff=0;
            }
        }
#endif
        if(-1!=(c=ozserialgetc()))
        {
#ifdef SHORTCUT
            extern byte esc_seq;
            if(!esc_seq && (unsigned char)c>=32)
                ttyprint(c);
            else
#endif
                termputch(c);
        }
        if(kbhit()) get_key();
    }
/*
    sprintf(debugbuf,"Buffer max: %u\n",(unsigned int)maxbuf);
    termputs(debugbuf);
    ozgetch();
    ozgetch();
    ozgetch();
*/
    ozsetlcr(saved_lcr);
    ozsetbaud(saved_baud);
    ozportout(7,port7);
}
