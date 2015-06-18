#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_VALUES 65536

typedef struct
{
    char name[64];
    unsigned int length:2;
    unsigned int bss:1;
    unsigned int value:16;
    unsigned int located:1;
} val_t;

val_t values[MAX_VALUES];
int numvalues=0;
int opts=0;

int number(char *s)
{
    if(!*s) return 0;
    while(*s)
    {
        if(!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}

int stored(char *s)
{
    int i;
    char z[64];
    strcpy(z,s);
    z[strlen(z)-1]=0;
    for(i=0;i<numvalues;i++) if(!strcmp(values[i].name,z)) return 1;
    return 0;
}


void emit_fixed(char *s)
{
    char *t,*t2;
    int i;
    if(NULL==(t=strchr(s,'(')))
    {
        puts(s);
        return;
    }
    t2=strchr(s,')');
    if(NULL==t2)
    {
        puts(s);
        return;
    }
    if(NULL!=strchr(t2,','))
    {
        puts(s);  /* wrong side */
        return;
    }
    *t2=0;
    for(i=0; i<numvalues; i++)
    {
        if(!strcmp(t+1,values[i].name))
        {
            if(values[i].length==2 && t[-2]=='a')
                break;
            if(values[i].located)
            {
                *t2=')';
                puts(s);
                return;
            }
            *t=0;
            t2++;
            printf("%s%d%s\n"
                   "%s equ $-%d\n",s,values[i].value,t2,values[i].name,
                     values[i].length);
            values[i].located=1;
            opts++;
            return;
        }
    }
    *t2=')';
    puts(s);
    return;
}

char *gets0(char *line) /* skip global lines */
{
    do
    {
        if(NULL==gets(line)) return NULL;
    } while(!strncmp(line,"global ",7));
    return line;
}



main()
{
    char line[1024],line2[1024],line3[1024];
    int i,l;
    fprintf(stderr,"miniopt2... ");
    setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
    while(NULL!=gets0(line))
    {
TOP:
        if(line[strlen(line)-1]==':' && (isalpha(line[0])
             || line[0]=='_'))
        {
            if(NULL==gets0(line2))
            {
                break;
            }
            else if((strncmp(line2,"defw",4)
             && strncmp(line2,"defb",4)
             && strncmp(line2,"defs",4))
             || !number(line2+5))
            {
                continue;
            }
            else if(NULL==gets0(line3))
            {
                break;
            }
            else if(!strncmp(line3,"def",3) && line3[4]==' ')
            {
                continue;
            }
            else if(line2[3]=='w')
            {
                line[strlen(line)-1]=0;
                strcpy(values[numvalues].name,line);
                if(numvalues>=MAX_VALUES)
                {
                    fprintf(stderr,"miniopt2: Fatal error!");
                    exit(4);
                }
                values[numvalues].length=2;
                values[numvalues].bss=0;
                values[numvalues].value=atoi(line2+5);
                values[numvalues].located=0;
                numvalues++;
                strcpy(line,line3);
                goto TOP;
            }
            else if(line2[3]=='b')
            {
                line[strlen(line)-1]=0;
                strcpy(values[numvalues].name,line);
                values[numvalues].length=1;
                values[numvalues].bss=0;
                values[numvalues].value=(unsigned char)atoi(line2+5);
                values[numvalues].located=0;
                numvalues++;
                strcpy(line,line3);
                goto TOP;
            }
            else if(line2[3]=='s')
            {
                l=atoi(line2+5);
                if(l!=1 && l!=2)
                {
                    strcpy(line,line3);
                    goto TOP;
                }
                line[strlen(line)-1]=0;
                strcpy(values[numvalues].name,line);
                values[numvalues].length=l;
                values[numvalues].bss=1;
                values[numvalues].value=0;
                values[numvalues].located=0;
                numvalues++;
                strcpy(line,line3);
                goto TOP;
            }
        }
    }
    rewind(stdin);
    while(NULL!=gets(line))
    {
        if(line[strlen(line)-1]==':' && (isalpha(line[0])
             || line[0]=='_') && stored(line))
        {
            if(NULL==gets(line)) continue;
        }
        else emit_fixed(line);
    }
    puts("psect data");
    for(i=0;i<numvalues;i++)
    {
        if(!values[i].located && !values[i].bss)
        {
            printf("%s: def%c %d\n",values[i].name,
                values[i].length==1?'b':'w',
                values[i].value);
        }
    }
    puts("psect bss");
    for(i=0;i<numvalues;i++)
    {
        if(!values[i].located && values[i].bss)
        {
            printf("%s: defs %d\n",values[i].name,
                values[i].length);
        }
    }
    fprintf(stderr,"%d optimizations\n",opts);
    return 0;
}



