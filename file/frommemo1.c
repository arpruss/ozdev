#include <string.h>
#include <oz.h>

extern char filebuf[];
extern char yn[];
extern char anykey[];

char *utoa_0n(unsigned u,int ndigits)
{
    static char buffer[11];
    register char *b;
    if(ndigits>10) return NULL;
    b=buffer+10;
    *b=0;
    while(ndigits--)
    {
        b--;
        *b=u%10+'0';
        u/=10;
    }
    return b;
}

/*
void emit(char c)
{
    static byte x=0;
    static byte y=0;
    if(x>231)
    {
        x=0;
        y+=8;
        if(y>72)
        {
            ozcls();
            y=0;
        }
    }
    x=ozputch(x,y,c);
}
*/

void frommemo(void)
{
    static unsigned loc,page,offset,recnum;
    register char *p;
    static char filename[32];
    static char parthead[21];
    static char continued;
    static unsigned int sum;
    static byte val;
    static int i;
    static int c;
    static byte x;
    static int partnumber;
    static char checksum[6];
    page=0;
    offset=0;
    while(loc=ozfindnext(TYPE_MEMO,&page,&offset,&recnum))
    {
        partnumber=0;
        p=ozloadcluster(loc);
        if(p[24]!='~' || p[25]!='~'
         || p[26]!='z'
         || strncmp(p+2+13+9+15,"00000",5)
         || strncmp(p+2+13+9+21,"**LIBOZ_UPLOAD*",15)) continue;
        do
        {
            strcpy(parthead,p+24);
            if(ozopenfile(loc)==-1)
            {
              ozwarn("Error opening memo!",anykey);
              return;
            }
            for(i=0;i<13+9+21+15;i++) ozreadfilebyte();
            continued=ozreadfilebyte();
            p=filename;
            while('\r'!=(c=ozreadfilebyte()) && '\n'!=c && p-filename<MAX_FILENAMELEN)
            {
                if(partnumber)
                {
                    if(c!=*p)
                    {
                        ozputs(0,70,"You may have the remains of "
                        "another download.");
                        ozclosefile();
                        close1();
                        unlink(filename);
                        ozgetch();
                        return;
                    }
                }
                else *p=c;
                p++;
            }
            if(p-filename>MAX_FILENAMELEN)
            {
                ozwarn("Filename too long!",anykey);
                return;
            }
            *p=0;
            ozputs(0,10,filename);
            ozputs(0,20,parthead);
            if(!partnumber)
            {
                ozsetowner(atoi(parthead+3));
                if(open1(filename,O_WRONLY)==-1)
                {
                    ozwarn("Error opening file!",anykey);
                    return;
                }
            }
            sum=0;
            p=filebuf;
            while(-1!=(c=ozreadfilebyte()) && c!='*')
            {
                /*emit(c);*/
                if(c=='~')
                {
                    while(-1!=(c=ozreadfilebyte()) && '~'!=c)
                    {
                        /*emit(c);*/
                        sum+=(byte)c;
                        *p++=c;
                    }
                }
                else
                {
                    if('0'<=c && c<='9') val=(c-'0')<<4;
                      else val=(c+(10-'a'))<<4;
                    c=ozreadfilebyte();
                    if('0'<=c && c<='9') val+=(c-'0');
                      else val+=(c+(10-'a'));
                    *p++=val;
                    sum+=(byte)val;
                }
            }
            if('*'==(c=ozreadfilebyte()))
            {
                for(i=0;i<5;i++) checksum[i]=ozreadfilebyte();
                checksum[5]=0;
            }
/*
            {
                extern unsigned didnonstandard;
                if(didnonstandard)
                {
                    ozputs(0,0,utoa_0n(loc,5));
                    ozputs(120,0,utoa_0n(didnonstandard,5));
                    ozputs(0,70,"NONSTANDARD!");
                    ozgetch();
                    didnonstandard=0;
                }
            }
*/
            if('*'!=c || (unsigned)(atoi(checksum))!=sum)
            {
#if 0
                if('*'!=c)
                {
/*                extern unsigned didnonstandard;
                    ozputs(0,0,utoa_0n(loc,5));
                    ozputs(120,0,utoa_0n(didnonstandard,5));
*/
                    x=ozputs(0,70,"Bad terminator: ");
                    ozputs(x,70,utoa_0n(c,5));
                }
                else
                {
                    x=ozputs(0,70,"Bad checksum ");
                    x=ozputs(x,70,utoa_0n(sum,5));
                    x=ozputs(x,70,", not ");
                    ozputs(x,70,checksum);
                }
#endif
                ozclosefile();
                close1();
                unlink(filename);
                ozwarn("Error in memo!",anykey);
                return;
            }
            ozclosefile();
            if(write1(filebuf,p-filebuf) < (p-filebuf) )
            {
               close1();
               unlink(filename);
               ozwarn("Error in writing!",anykey);
               return;
            }
            if(ozunlink(TYPENUM_MEMO,recnum))
            {
                if(ozwarn("Error unlinking!  Continue?",yn)!=KEY_LOWER_ENTER) return;
            }
/*            ozputch(0,0,continued); */
            if(continued=='L')
                break;
            partnumber++;
            strcpy(parthead+15,utoa_0n(partnumber,5));
            page=offset=0;
            while(loc=ozfindmemo(parthead,&page,&offset,&recnum))
            {
                p=ozloadcluster(loc);
                if(strncmp(p+2+13+9+21,"**LIBOZ_UPLOAD*",15)) continue;
                break;
            }
            if(loc==0)
            {
                close1();
                unlink(filename);
                ozwarn("Error: Cannot find next part!",anykey);
                return;
            }
        } while(loc);
        close1();
        page=offset=0;
    }
}


