#include <oz.h>
#include <ozmenu.h>

struct ozmenuentry menu[]=
{
    { 'l', "Last name" },
    { 'f', "First name" },
    { 'c', "Company" }
};

main()
{
    static int m;
    static unsigned c;
    c=ozgetindexby(TYPENUM_TELEPHONE)-1;
    m=ozmenu(0,11,"Order telephone entries by",
       c,3,menu,OZMENU_NUMBERED);
    if(m>=0)
    {
        ozsetindexby(TYPENUM_TELEPHONE,m+1);
        ozexitto(KEY_TELEPHONE);
    }
}
