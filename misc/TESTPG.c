#include <oz.h>

void setpage(unsigned p)
{
  ozportout(3,p&0xff);
  ozportout(4,p>>8);
}

void checkpage(byte line,unsigned page)
{
    register unsigned *x=(void*)0xa000;
    unsigned i;
    setpage(page);
    for(i=0;i<4096;i++)
    {
        if(x[i]!=i*i+0x4221+i)
        {
            ozputs(0,line,"Error in readback!");
            return;
        }
    }
    ozputs(0,line,"OK!");
}

void fillpage(unsigned page)
{
    register unsigned *x=(void*)0xa000;
    unsigned i;
    setpage(page);
    for(i=0;i<4096;i++)
        x[i]=i*i+0x4221+i;
}


main()
{
   checkpage(0,0x404);
   checkpage(10,0x405);
   ozputs(0,20,"Fill?");
   if(ozgetch()!='y') return;
   fillpage(0x404);
   fillpage(0x405);
   return;
}

