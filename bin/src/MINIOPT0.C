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
    setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
    fprintf(stderr,"miniopt0... ");
    while(NULL!=gets(line1))
    {
TOP:
        if(!strcmp(line1,"ex\tde,hl"))
        {
            if(NULL==gets(line2))
            {
                puts(line1);
                getout();
            }
            else
            if(strncmp(line2,"ld\thl,",6))
            {
                puts(line1);
                puts(line2);
            }
            else
            if(NULL==gets(line3))
            {
                puts(line1);
                puts(line2);
                getout();
            }
            else
            if(/* strcmp(line3,"add\thl,de") && */ strcmp(line3,"call\twrelop"))
            {
                puts(line1);
                puts(line2);
                strcpy(line1,line3);
                goto TOP;
            }
            else
            {
/*                if(!strcmp(line3,"call\twrelop")) */
                    printf("global wrelop_ex\n"
                           "ld\tde,%s\n"
                           "call\twrelop_ex\n",
                           line2+6);
/*                else
                   printf("ld\tde,%s\n"
                          "add\thl,de ;; *miniopt0*\n",line2+6); */
                opts++;
            }
        }
        else if(!strcmp(line1,"pop\tbc"))
        {
            popbcs=0;
            do
            {
                popbcs++;
                if(NULL==gets(line1))
                {
                    for(i=0;i<popbcs;i++)
                        puts("pop\tbc");
                    getout();
                }
            } while(!strcmp(line1,"pop\tbc"));
            if(strcmp(line1,"jp\tcret"))
            {
                for(i=0;i<popbcs;i++)
                    puts("pop\tbc");
                goto TOP;
            }
            puts("jp\tcret");
            opts++;
        }
        else puts(line1);
    }
    getout();
    return 0;
}
