#include <oz.h>
#include <stdlib.h>
#include <string.h>
#include "wizbook.h"

#define MAX_VERSE 255
#define MAX_CHAPTER 255
extern char *long_names_offset;
extern byte *list_of_chapters_offset;
extern byte number_of_books;

unsigned index_offset;
unsigned index_area_length;

/* index data:
        unsigned long_name_offset;
        unsigned list_of_chapters_offset;
        byte: number of books
        list: of short names (each asciiz)
        list: of long names (each asciiz)
        list of chapters:
            byte: starting chapter number
            byte: ending chapter number
            list:
                  byte*3: locations of chapters
                  byte:   number of verses
*/

void do_goto(void)
{
    register char *s;
#define b ((byte*)s)
    static char *chapter_num,*verse_num;
    static byte bookname_len;
    static byte best_match_len;
    static byte f;
    static char *t;
    byte chapter,verse;
    byte i;
    byte book;
    byte l;
    byte ll;
    byte ofontheight;
    byte omyfont;
    byte odorev;

    if(!index_area_length) return;
    ofontheight=font_height;
    omyfont=myfont;
    odorev=dorev;
    font_height=8;
    myfont=FONT_OZ_NORMAL;
    dorev=0;
    getsearchbuf();
    myfont=omyfont;
    font_height=ofontheight;
    dorev=odorev;
    if(!*searchbuf) return;
    myseek(index_offset);
    myread_check(filebuf,index_area_length);
    long_names_offset=(char*)filebuf+(unsigned)long_names_offset;
    list_of_chapters_offset=(unsigned)list_of_chapters_offset+filebuf;
    s=searchbuf;
    while(isdigit(*s)) s++;
    while(isalpha(*s)) s++;
    bookname_len=s-searchbuf;
    if(bookname_len)
    {
        while(*s==' ') s++;
        chapter_num=s;
        while(isdigit(*s)) s++;
        while(*s && !isdigit(*s)) s++;
        verse_num=s;
        s=(char*)filebuf+5; /* short names */
        book=1;
        for(i=0;i<number_of_books;i++)
        {
            if(!strncmp(s,searchbuf,bookname_len))
            {
                book=i;
                break;
            }
            s+=strlen(s)+1;
        }
        if(i==number_of_books)
        {
            best_match_len=0;
            for(i=0;i<number_of_books;i++)
            {
                ll=0;
                l=bookname_len;
                t=searchbuf;
                while(l-- && *s++==*t++) ll++;
                if(ll>best_match_len)
                {
                    best_match_len=ll;
                    book=i;
                }
                s+=strlen(s)+1;
            }
        }
        chapter=atoi(chapter_num);
        verse=atoi(verse_num);
        s=(char*)list_of_chapters_offset;
        for(i=0;i<book;i++)
            s+=(unsigned int)(b[1]-b[0]+1)*3;
        if(chapter<b[0])
        {
            chapter=b[0];
            verse=1;
        }
        else if(chapter>b[1])
        {
            chapter=b[1];
            verse=MAX_VERSE;
        }
        s+=4*(unsigned)(chapter-b[0]);
        if(verse>b[3]) verse=b[3];
        if(!verse) verse=1;
        memcpy(&position0,b,3);
    }
    reload(index_area_length);
    if(!bookname_len) return;
    goto_pos(position0);
    while(--verse)
    {
        while((f=forwardline()) && filebuf[screen_offset]==0);
        if(!f) return;
    }
}
