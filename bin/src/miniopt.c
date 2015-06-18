/* Optimizes "zxc -s" output by removing redundant zero loads,
   changing them to register loads when needed, and removing
   double ex de,hl pairs.  Not much of an optimization, but
   better than nothing.  Can be used when the Hi-Tech C optimizer
   runs out of memory. */
#define SRL
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define NUMREGS ('l'-'a'+1)
int iszero[NUMREGS];
int opts=0;

#if 0
int atoi(char *s)
{
    int sign=1;
    int value=0;
    if(*s=='-')
    {
        sign=-1;
        s++;
    }
    while(isdigit(*s))
    {
        value=value*10+(*s-'0');
        s++;
    }
    return value;
}
#endif

int isnumber(char *s)
{
    if(*s=='-') s++;
    if(!*s || !isdigit(*s)) return 0;
    while(*++s && *s!='\n')
        if(!isdigit(*s)) return 0;
    return 1;
}

int with_acc(char *s)
{
    if(
      !strncmp(s,"or ",3)
     || !strncmp(s,"and ",4) || !strncmp(s,"xor ",4)
                   || !strncmp(s,"sub ",4)
                   || !strncmp(s,"rla",3)
                   || !strncmp(s,"rra",3)
                   || !strncmp(s,"rrca",4)
                   || !strncmp(s,"rlca",4)
                   || !strncmp(s,"rrd",3)
                   || !strncmp(s,"rrl",3)
                   )

                   return 1;
    return 0;
}

char *getline(char *buf)
{
    char op[80],arg1[80];
    char *buf0;
    int i;
    if(NULL==gets(buf)) return NULL;
    for(i=0;buf[i];i++)
    {
        if(buf[i]=='\t') buf[i]=' ';
        if(buf[i]==';') buf[i]=0;
    }
    for(i=strlen(buf)-1; i>=0 && buf[i]==' ' ;i--)
      buf[i]=0;
    if(!*buf) return buf;
    if(!strcmp(buf,"call wrelop") && iszero['h'-'a'] && iszero['d'-'a'])
    {
        strcpy(buf,"ld a,l\n"
                   "cp e  ;; *miniopt* were call wrelop");
        iszero['a'-'a']=iszero['l'-'a'];
        opts++;
        return buf;
    }
    if(!strcmp(buf,"call wrelop_ex") && iszero['h'-'a'] && iszero['d'-'a'])
    {
        strcpy(buf,"ld a,e\n"
                   "cp l  ;; *miniopt* were call wrelop_ex");
        iszero['a'-'a']=iszero['e'-'a'];
        opts++;
        return buf;
    }
    if(!strncmp(buf,"push",4)) return buf;
    if(!strcmp(buf,"or a")) return buf;
    if(!strcmp(buf,"neg") || !strcmp(buf,"cpl")
    || with_acc(buf))
    {
        iszero['a'-'a']=0;
        return buf;
    }
    if(!strncmp(buf,"ldi",3) || !strncmp(buf,"cpi",3)
     || !strncmp(buf,"outi",4) || !strncmp(buf,"ini",3)
     || !strncmp(buf,"ldd",3) || !strncmp(buf,"cpd",3)
     || !strncmp(buf,"outd",4) || !strncmp(buf,"ind",3))
    {
        iszero['h'-'a']=iszero['l'-'a']=iszero['b'-'a']=iszero['c'-'a']
         = iszero['d'-'a']=iszero['e'-'a']=0;
        return buf;
    }
#ifdef SRL
    if(!strncmp("srl ",buf,4))
    {
        return buf;
    }
#endif
    if(!strncmp(buf,"ex",2))
    {
        if(!strcmp(buf,"ex de,hl"))
        {
            int sw;
            sw=iszero['h'-'a'];
            iszero['h'-'a']=iszero['d'-'a'];
            iszero['d'-'a']=sw;
            sw=iszero['l'-'a'];
            iszero['l'-'a']=iszero['e'-'a'];
            iszero['e'-'a']=sw;
        }
        else if(!strcmp(buf,"exx"))
          for(i=1;i<NUMREGS;i++) iszero[i]=0; /* unknown: all but a */
        else if(!strcmp(buf,"ex (sp),hl"))
        {
            iszero['h'-'a']=0;
            iszero['l'-'a']=0;
        }
        else iszero['a'-'a']=0;  /* ex af,af' */
        return buf;
    }
    if(!strncmp(buf,"djnz",4))
    {
        iszero['b'-'a']=0;
        return buf;
    }

    if(NULL!=strchr(buf,':') || !strncmp("call",buf,4)
     || !strncmp("jr",buf,2) || !strncmp("jp",buf,2)
     || !strncmp("ret",buf,3))
    {
        buf0=NULL;
        if(!strcmp(buf,"call lmul") || !strcmp(buf,"call amul"))
        {
            if(iszero['h'-'a'])
            {
               buf0="call lmul$h0 ;; *miniopt* was call lmul\n"
                   "global lmul$h0";
               opts++;
            }
            else if(iszero['d'-'a'])
            {
               buf0="call lmul$d0 ;; *miniopt* was call lmul\n"
                   "global lmul$d0";
               opts++;
            }
        }
        for(i=0;i<NUMREGS;i++) iszero[i]=0;
        if(buf0!=NULL) strcpy(buf,buf0);
        return buf;
    }
    if(!strncmp("ld ",buf,3) && !strcmp(buf+4,",0")) return buf;
    if(!strncmp("ld ",buf,3) && buf[4]==',' && isalpha(buf[3])
      && isalpha(buf[5]))
    {
        if(iszero[buf[3]-'a'] && iszero[buf[5]-'a'])
            sprintf(buf,"ld %c,0",buf[3]);
        else
            iszero[buf[3]-'a']=iszero[buf[5]-'a'];
        return buf;
    }
    if(!strncmp("ld ",buf,3) && isalpha(buf[3]) && isalpha(buf[4])
      && buf[5]==',' && buf[3]!='i' && isnumber(buf+6))
        /* ld mn,... */
    {
        return buf;
    }
    if(!strncmp("set ",buf,4))
    {
        if(isalpha(buf[6])) iszero[buf[6]-'a']=0;
        return buf;
    }
    *arg1=0;
    *op=0;
    sscanf(buf,"%s %s",op,arg1);
    if(isalpha(*arg1))
    {
        iszero[*arg1-'a']=0;
        if(isalpha(*arg1))
         iszero[arg1[1]-'a']=0;
    }
    return buf;
}

int main()
{
    char buf[1024];
    char buf2[1024];
    char buf3[1024];
    int i;
    int text=0;
    unsigned short x1,x2;
    unsigned short val;
   setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
    fprintf(stderr,"miniopt... ");
    while(NULL!=getline(buf))
    {
TOP:
#ifdef DEBUG
        for(i=0;i<NUMREGS;i++)
            if(iszero[i]) fprintf(stderr,"%c",i+'a');
        fprintf(stderr,"\n");
#endif

        if(!strncmp(buf,"ld ",3) && !strcmp(buf+4,",0"))
        {
            if(iszero[buf[3]-'a'])
            {
                printf(";;*miniopt* removed %s\n",buf);
                opts++;
                continue;
            }
            for(i=0;i<NUMREGS;i++)
                if(iszero[i])
                {
                    printf("ld %c,%c ;;*miniopt* was ld %c,0\n",buf[3],i+'a',buf[3]);
                    opts++;
                    break;
                }
            if(i==NUMREGS)
            {
                puts(buf);
            }
            iszero[buf[3]-'a']=1;
        }
#ifdef SRL
        else
        if(!strncmp(buf,"srl ",4) && isalpha(buf[4]) && iszero[buf[4]-'a'])
        {
            if(NULL==getline(buf2))
            {
                puts(buf);
                return 0;
            }
            if(strncmp(buf2,"rr ",3))
            {
                puts(buf);
                strcpy(buf,buf2);
                goto TOP;
            }
            printf("srl %s ;; *miniopt*\n",buf2+3);
            opts++;
        }
#endif
        else
        if(!strcmp(buf,"ex de,hl"))
        {
            if(NULL==getline(buf2))
            {
                puts(buf);
                return 0;
            }
            if(strcmp(buf2,"ex de,hl"))
            {
                puts(buf);
                puts(buf2);
            }
            else
                {
                    opts++;
                    puts(";;*miniopt* removed double ex de,hl");
                }

        }
        else
        if(!strncmp(buf,"ld ",3)
           && buf[5]==','
           && isalpha(buf[3])
           && isalpha(buf[4])
           && buf[3]!='i' && isnumber(buf+6))
          /* ld ab,NNNN */
        {
            val=atoi(buf+6);
            x1=val & 0xFF;
            x2=val >> 8;
            if(!val)
            {
                if(iszero[buf[3]-'a'] && iszero[buf[4]-'a'])
                {
                    printf(";;*miniopt* removed %s\n",buf);
                    opts++;
                }
                else
                {
                  for(i=0;i<NUMREGS;i++)
                    if(iszero[i])
                    {
                        if(iszero[buf[3]-'a'])
                        {
                          printf("ld %c,%c ;;*miniopt* was %s\n",
                                buf[4],i+'a',buf);
                          opts++;
                        }
                        else
                          if(iszero[buf[4]-'a'])
                          {
                           printf("ld %c,%c ;;*miniopt* was %s\n",
                                buf[3],i+'a',buf);
                           opts++;
                          }
                        else
                        {
                           printf("ld %c,%c ;;*miniopt*\n"
                                  "ld %c,%c ;;*miniopt* were %s\n",
                                  buf[3],i+'a',buf[4],i+'a',buf);
                           opts++;
                        }
                        break;
                    }
                  if(i==NUMREGS)
                    puts(buf);
                }
            }
            else
            {
               if(!x1 && iszero[buf[4]-'a'])
               {
                 printf("ld %c,%u ;;*miniopt* was %s\n",
                      buf[3],x2,buf);
                 opts++;
               }
               else if(!x2 && iszero[buf[3]-'a'])
               {
                 printf("ld %c,%u ;;*miniopt* was %s\n",
                      buf[4],x1,buf);
                 opts++;
                }
               else puts(buf);
            }
            if(!x1) iszero[buf[4]-'a']=1;
             else iszero[buf[4]-'a']=0;
            if(!x2) iszero[buf[3]-'a']=1;
             else iszero[buf[3]-'a']=0;
        }
        else puts(buf);
    }
    fprintf(stderr,"%d optimizations\n",opts);
    return 0;
}


