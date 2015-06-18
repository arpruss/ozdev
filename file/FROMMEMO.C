#include <string.h>
#include <stdlib.h>
#include <oz.h>


unsigned ozwarn(char *s1,char *s2);

#ifdef STANDALONE
#include "warn.c"
char filebuf[2400];
char anykey[]="Press any key";
char yn[]="Yes:[ENTER]/No:[ESC]";
#else
extern char filebuf[];
extern char yn[];
extern char anykey[];
#endif

#define EOF_MARKER 33
#define HEX 35
#define SPECIAL (HEX+16)
#define NUMSPECIALS (254-SPECIAL)
#define TOMEMO2_MARKER 254

byte table[NUMSPECIALS];

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

#ifdef STANDALONE
main()
#else
void frommemo(void)
#endif
{
    static unsigned loc,page,offset,recnum;
    register char *p;
    static char filename[32];
    static char parthead[21];
    static char continued;
    static unsigned int sum;
    static byte val;
    static byte version2;
    static int i;
    static unsigned j;
    static int c;
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
            if(!partnumber)
            {
                c=ozreadfilebyte();
                if(c==TOMEMO2_MARKER)
                {
                   version2=1;
                   for(j=0;j<NUMSPECIALS;j++)
                   {
                      c=ozreadfilebyte();
                      table[j]=(c-HEX)|((ozreadfilebyte()-HEX)<<4);
                   }
                }
                else
                {
                   version2=0;
                   goto PROCESS_VERSION1;
                }
            }
            if(version2)
            {
                while(-1!=(c=ozreadfilebyte()) && (byte)c!=EOF_MARKER)
                {
                    if((byte)c>=SPECIAL)
                        val=table[c-SPECIAL];
                    else
                        val=(c-HEX)|((ozreadfilebyte()-HEX)<<4);
                    sum+=val;
                    *p++=val;
                }
                if(c==EOF_MARKER)
                   c='*';
            }
            else
            {
                while(-1!=(c=ozreadfilebyte()))
                {
                    PROCESS_VERSION1:
                    if((byte)c=='*') break;
                    if((byte)c=='~')
                    {
                        while(-1!=(c=ozreadfilebyte()) && '~'!=(byte)c)
                        {
                            sum+=(byte)c;
                            *p++=c;
                        }
                    }
                    else
                    {
                        if('0'<=(byte)c && (byte)c<='9') val=(c-'0')<<4;
                          else val=(c+(10-'a'))<<4;
                        c=ozreadfilebyte();
                        if('0'<=(byte)c && (byte)c<='9') val+=(c-'0');
                          else val+=(c+(10-'a'));
                        *p++=val;
                        sum+=(byte)val;
                    }
                }
                c=ozreadfilebyte();
            }
            if('*'==c)
            {
                for(i=0;i<5;i++) checksum[i]=ozreadfilebyte();
                checksum[5]=0;
            }
            if('*'!=c || (unsigned)(atoi(checksum))!=sum)
            {
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


