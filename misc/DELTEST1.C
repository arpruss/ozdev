#include <oz.h>

main()
{
    char buf[25];
    unsigned page=0,offset=0,recnum=0;
    if(!ozfindmemo("Test9",&page,&offset,&recnum))
    {
      ozputs(10,10,"Not found!");
      ozgetch();
      return;
    }
    sprintf(buf,"%d :: %d",recnum,ozunlink(TYPENUM_MEMO,recnum));
    ozputs(10,10,buf);
    ozgetch();
    return;
}
