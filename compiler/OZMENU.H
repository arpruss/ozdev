#ifndef _OZMENU_H
#define _OZMENU_H

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

struct ozmenuentry
{
    unsigned key;
    char *label;
};

int ozmenu(int x,int y,char *title,unsigned start,unsigned num_entries,
    struct ozmenuentry *menu, byte options);
#define OZMENU_NUMBERED 1
#define OZMENU_NOEXIT   2
#define OZMENU_NOLIGHT  4
#endif /* _OZMENU_H */
