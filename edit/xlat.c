#include <oz.h>
#include "edit.h"

#ifdef TRANSL

#define HI(x) (*(1+(byte*)(&(x))))
#define LO(x) ((byte)(x))

static byte inner(void)
{
    extern byte _ozclick_setting;
    if(_ozclick_setting) ozkeyclick();
    switch(getch_cursor())
    {
        case 'e': return 0;
        case 'o': return 1;
        case 'u': return 2;
        case 'a': return 3;
        case 'E': return 4;
        case 'O': return 5;
        case 'U': return 6;
        case 'A': return 7;
        default: return 0xFF;
    }
}

static byte accentit(byte offset,byte num)
{
    static byte d;
    d=inner();
    if(d<num) return d+offset; else return 0xff;
}

byte xlat(unsigned k)
{
    if(HI(k)!=(MASK2ND >> 8)) return LO(k);
    switch(LO(k))
    {
        case 'k':
            return accentit(0,4);
        case 'j':
            return accentit(4,4);
        case 'n':
            return accentit(14,6);
        case 'b':
            return accentit(20,7);
        case ',':
            return 28;
        case '.':
            return 12;
        case 'm':
            return 29;
        case 'l':
            switch(getch())
            {
                case 'n': return 31;
                case 'N': return 10;
                default: return 0xFF;
            }
        case '0':
            return 8;
        case '-':
            return 9;
        case 'y':
            return 11;
        case '3':
            return 30;
        default:
            return 0xFF;
    }
}

/*
main()
{
    xlat(getch());
}
*/

#endif /* TRANSL */
