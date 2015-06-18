#include <stdio.h>

int opts=0;

void getout(void)
{
    fprintf(stderr,"%d optimizations\n",opts);
    exit(0);
}

main()
{
    char line1[1024],line2[1024],line3[1024];
    int popbcs,i;
    int c1;
    int d1,d2,d3,d4,d5;
    setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
   fprintf(stderr,"miniopt1... ");
    while(NULL!=gets(line1))
    {
TOP:
        if(!(c1=strncmp(line1,"call wrelop",11)) ||
            !strcmp(line1,"call brelop"))
        {
            if(NULL==gets(line2))
            {
                puts(line1);
                getout();
            }
            else
            if((d1=strncmp(line2,"jp c,",5)) &&
            (d2=strncmp(line2,"jp nc,",6)) &&
            (d3=strncmp(line2,"jp z,",5))
            && (d4=strncmp(line2,"call c,",7))
            && (d5=strncmp(line2,"call nc,",8)))
            {
                puts(line1);
                puts(line2);
            }
            else
            {
                opts++;
                if(line1[5]=='b') puts("cp b ;; *miniopt1*");
                else
                {
                  puts("or a");
                  if(!strncmp(line1,"call wrelop_ex",14)
                  && strncmp(line2,"jp z,",5))
                    puts("ex de,hl");
                  puts("sbc hl,de ;; *miniopt1*");
                }
                puts(line2);
            }
        }
        else puts(line1);
    }
    getout();
    return 0;
}
