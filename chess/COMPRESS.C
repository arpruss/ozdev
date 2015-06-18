/* a little string compressor */
/* won't handle all strings but good enough for quick and dirty stuff */
/* all it does is set the high bit of a character in a string if the next one is a space */

#include <stdio.h>
#include <string.h>

main()
{
    static char line0[1025]="\0";
#define line (line0+1)
    char *p,*p2,*q;
    while(NULL!=gets(line))
    {
        p=strchr(line,'"');
        if(p==NULL || p[-1]=='\'' || p[-1]=='\\')
        {
            puts(line);
            continue;
        }
        p2=strchr(p+1,'"');
        if(p2==NULL)
        {
            puts(line);
            continue;
        }
        q=line;
        while(q<=p)
        {
            putchar(*q);
            q++;
        }
        while(q<p2)
        {
            if(q[1]==' ')
            {
                printf("\\x%X\"\"",(unsigned char)q[0]|128);
                q++;
            }
            else
            {
                putchar(*q);
            }
            q++;
        }
        while(*q)
        {
            putchar(*q);
            q++;
        }
        putchar('\n');
    }
}



