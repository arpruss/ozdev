/*
  This file contains the routines that will need to be adjusted,
  ported, rewritten, or written in assembly.

  With the use of the included stdlib.c file, all you should need is to
  write the various support routines in this file, and include any math
  operations, such as 16 bit multiply, divide and remainder (modulus),
  and of course, any startup code the compiler needs.
*/
#include <ozinput.h>
#include <oztime.h>

#define HEADER
#include "arvar.c"

char *itoa(int u, char *s)
{
    static char buffer[10];
    static byte sign;
    register char *b;
    if(u<0)
    {
        sign=1;
        u=-u;
    }
    else sign=0;
    b=buffer+9;
    *b=0;
    while(u || b==buffer+9)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    if(sign) *--b='-';
    strcpy(s,b);
    return s;
}

void termputs(char *_s)
{
    register char *s=_s;
    static uchar i;
    while(*s)
    {
        termputch(*s);
        s++;
    }
}

void printf(char *fmt, int n)
{
    int *data;
    static char buf[10];
    data=&n;
    while(*fmt)
    {
        if(*fmt=='%')
           {
             ++fmt;
             switch(*fmt)
             {
                case 's': termputs((char*)(*data)); data++; break;
                case 'u':
                case 'd': termputs(itoa(*data,buf)); data++; break;
                case 0: return;
             }
           }
        else termputch(*fmt);
        fmt++;
    }
}

void clrscr(void)
{
    extern uchar curcol,currow;
    ozcls();
    curcol=currow=0;
}

void initprg(void)
/* just do what you need to do to get the program running.  This is in
   addition to whatever the startup code does */
/* I suggest that you turn the terminal page pause and auto echo off */
/* you may also need to tie into the clock interupt here.   That should
   just increment a time structure clock, or something */
{
ozsinglepage();
resetclock();
}

/* sound a bell, or beep */
void Bell(void) {
/* don't want to include dos.h just for these. */
ozsound(5);
ozdelay64hz(16);
ozquiet();
}

/* since you may not be able to actually reset the clock to zero, I
   provide a 'zero' clock reference to subtract from the current time */

void resetclock(void)
/* 'reset' the zero clock */
{
    extern unsigned _oz64hz_word[];
    _oz64hz_word[0]=_oz64hz_word[1]=0;
    ElapClock=0;timeout=FALSE;
}

void elaptime(void)
/* this finds the elapsed time since 'zerotime',
   leaving the result in 'time' struct */
/* This routine allows for you to NOT have a free interupt that you
   can use for a clock interupt call to irqclock().  */
{
static unsigned k;
ElapClock=gettime();
if (ElapClock>=(responsetime+extratime)) timeout=TRUE;
if(breakctrl && ozkeyhit() && ( (k=ozgetch())==KEY_UPPER_ESC || k==KEY_LOWER_ESC) )
    timeout=tcontrol=1;
}



