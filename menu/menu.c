#include <oz.h>
#include <ozmenu.h>

struct ozmenuentry test_menu[]
={
    { 'a',"Entry A" },
    { 'b',"Entry B" },
    { 'c',"Entry C" },
    { 'd',"Entry D" },
    { 'e',"Entry E" },
    { 'f',"Entry F" },
    { 'g',"Entry G" },
    { 'h',"Entry H" },
    { 'i',"Entry I" }
};

main()
{
    int m;
    m=ozmenu(10,20,"A Test Menu",0,9,test_menu,0);
    if(m<0) return;
    ozputs(0,0,test_menu[m].label);
    ozsetfont(FONT_PC_LARGE);
    m=ozmenu(10,20,"A Test Menu",m,9,test_menu,OZMENU_NUMBERED);
    if(m<0) return;
    ozputs(0,10,test_menu[m].label);
    ozgetch();
}

