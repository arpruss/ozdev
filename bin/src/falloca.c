#include <stdio.h>
#include <string.h>
#include <ctype.h>


main()
{
    char line[1024];
    int i,l;
    int text;
    int allocainfunction;
    int hadcallncsv;
    char *s;
    char stacksizename[32];
    int ssn_length;
    setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
    text=0;
    while(NULL!=gets(line))
        if(!strncmp(line,"psect\t",6))
        {
            text=!strncmp(line+6,"text",4);
            puts(line);
        }
        else
        {
           if(text && line[0]=='_')
           {
               hadcallncsv=allocainfunction=0;
               ssn_length=0;
               puts(line);
           }
           else
           if(text && !hadcallncsv && !strncmp(line,"call\tncsv",9))
           {
               hadcallncsv=1;
               puts(line);
           }
           else
           if(text && hadcallncsv && !ssn_length && !strncmp(line,
           "defw\t",5))
           {
               sscanf(line+5,"%s",stacksizename);
               ssn_length=strlen(stacksizename);
               puts(line);
           }
           else
           if(ssn_length && !strncmp(line,stacksizename,ssn_length)
             && !strncmp(line+ssn_length,"\tequ\t",5))
           {
               if(!allocainfunction || atoi(line+ssn_length+4))
               {
                  puts(line);
               }
               else
               {
                  printf("%s\tequ\t-1\n",stacksizename);
               }
           }
           else
           if(NULL!=(s=strstr(line,"___x_alloca_19828a_")))
           {
               s[3]='y';
               if(text && strncmp(line,"global\t",7))
               {
                  if(!strncmp(line,"call\t",5))
                  {
                     allocainfunction=1;
                  }
                  else
                  {
                     fprintf(stderr,"Fatal error: Invalid alloca() reference.\n");
                     return 3;
                  }
               }
               puts(line);
           }
           else puts(line);
        }
   return 0;
}

