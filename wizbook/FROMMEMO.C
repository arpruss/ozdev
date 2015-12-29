#include <string.h>
#include <stdlib.h>
#include <oz.h>

#define VER2

unsigned ozwarn(char *s1,char *s2);

#ifdef STANDALONE
#include "warn.c"
char frommemo_buf[2000];
char anykey[]="Press any key";
char yn[]="Yes:[ENTER]/No:[ESC]";
char fmerror_part[]="Error: Can't find part.";
char fmerror_remains[]="You may have the remains of another upload.";
char fmerror_unlink[]="Error unlinking. Continue?";
char liboz_upload[]="**LIBOZ_UPLOAD*";
char fmerror_memoopen[]="Error opening memo.";
char fmerror_v1[]="V.1 file not supported.";
char fmerror_toolong[]="Filename too long.";
char fmerror_openoutput[]="Error opening output.";
char fmerror_writing[]="Error writing.";
char fmerror_memo[]="Error in memo.";
char zeros[]="00000";
#else
extern char fmerror_writing[];
extern char fmerror_memo[];
extern char fmerror_openoutput[];
extern char fmerror_toolong[];
extern char fmerror_v1[];
extern char fmerror_memoopen[];
extern char liboz_upload[];
extern char frommemo_buf[];
extern char yn[];
extern char anykey[];
extern char fmerror_part[];
extern char fmerror_remains[];
extern char fmerror_unlink[];
extern char zeros[];
#endif

#define EOF_MARKER 33
#define HEX 35
#define SPECIAL (HEX+16)
#define NUMSPECIALS (254-SPECIAL)
#define TOMEMO2_MARKER 254

#ifdef STANDALONE
byte frommemo_table[NUMSPECIALS];
#else
extern byte frommemo_table[NUMSPECIALS];
#endif

#ifdef STANDALONE
char *utoa_05(unsigned u)
{
    static char buffer[11];
    register char *b;
    byte ndigits=5;
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
#else
char *utoa_05(unsigned u);
#endif

#define filename fm_filename
#ifdef STANDALONE
# define ES static
#else
# define ES extern
#endif

    ES char filename[32];

void closeunlink(void)
{
    close1();
    unlink(filename);
}

#ifdef STANDALONE
main()
#else
void frommemo(void)
#endif
{
    static unsigned loc,page,offset,recnum;
    register char *p;
#define parthead fm_parthead
    ES char parthead[21];
    static char continued;
    static unsigned int sum;
    static byte val;
#ifndef VER2
    static byte version2;
#endif
    static int i;
    static unsigned j;
    static int c;
    static int partnumber;
    ES char checksum[6];
    extern char _ozclusterbuffer[];
    page=0;
    offset=0;
    while(loc=ozfindnext(TYPE_MEMO,&page,&offset,&recnum))
    {
        partnumber=0;
        ozloadcluster(loc);
        if(
         _ozclusterbuffer[24]!='~' || _ozclusterbuffer[25]!='~'
         || _ozclusterbuffer[26]!='z'
         || strncmp(_ozclusterbuffer+2+13+9+15,zeros,5)
         || strncmp(_ozclusterbuffer+2+13+9+21,liboz_upload,15)) continue;
        do
        {
            strcpy(parthead,_ozclusterbuffer+24);
            if(ozopenfile(loc)==-1)
            {
              ozwarn(fmerror_memoopen,anykey);
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
                        ozwarn(fmerror_remains,
                        anykey);
                        ozclosefile();
                        closeunlink();
                        return;
                    }
                }
                else *p=c;
                p++;
            }
            if(p-filename>MAX_FILENAMELEN)
            {
                ozwarn(fmerror_toolong,anykey);
                return;
            }
            *p=0;
            _ozfilledbox(0,60,230,20,0);
            ozputs(0,60,filename);
            ozputs(0,70,parthead);
            if(!partnumber)
            {
                ozsetowner(atoi(parthead+3));
                if(open1(filename,O_WRONLY)==-1)
                {
                    ozwarn(fmerror_openoutput,anykey);
                    return;
                }
            }
            sum=0;
            p=frommemo_buf;
            if(!partnumber)
            {
                c=ozreadfilebyte();
                if(c==TOMEMO2_MARKER)
                {
#ifndef VER2
                   version2=1;
#endif
                   for(j=0;j<NUMSPECIALS;j++)
                   {
                      c=ozreadfilebyte();
                      frommemo_table[j]=(c-HEX)|((ozreadfilebyte()-HEX)<<4);
                   }
                }
                else
                {
#ifdef VER2
                   ozwarn(fmerror_v1,anykey);
                   exit(0);
#else
                   version2=0;
                   goto PROCESS_VERSION1;
#endif
                }
            }
#ifndef VER2
            if(version2)
#endif
            {
                while(-1!=(c=ozreadfilebyte()) && (byte)c!=EOF_MARKER)
                {
                    if((byte)c>=SPECIAL)
                        val=frommemo_table[c-SPECIAL];
                    else
                        val=(c-HEX)|((ozreadfilebyte()-HEX)<<4);
                    sum+=val;
                    *p++=val;
                }
                if(c==EOF_MARKER)
                   c='*';
            }
#ifndef VER2
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
#endif
            if('*'==c)
            {
                for(i=0;i<5;i++) checksum[i]=ozreadfilebyte();
                checksum[5]=0;
            }
            if('*'!=c || (unsigned)(atoi(checksum))!=sum)
            {
                ozclosefile();
                closeunlink();
                ozwarn(fmerror_memo,anykey);
                return;
            }
            ozclosefile();
            if(write1(frommemo_buf,p-frommemo_buf) < (p-frommemo_buf) )
            {
               closeunlink();
               ozwarn(fmerror_writing,anykey);
               return;
            }
            if(ozunlink(TYPENUM_MEMO,recnum))
            {
                if(ozwarn(fmerror_unlink,yn)!=KEY_LOWER_ENTER) return;
            }
            if(continued=='L')
                break;
            partnumber++;
            strcpy(parthead+15,utoa_05(partnumber));
            page=offset=0;
            while(loc=ozfindmemo(parthead,&page,&offset,&recnum))
            {
                p=ozloadcluster(loc);
                if(strncmp(p+2+13+9+21,liboz_upload,15)) continue;
                break;
            }
            if(loc==0)
            {
                closeunlink();
                ozwarn(fmerror_part,anykey);
                return;
            }
        } while(loc);
        close1();
        page=offset=0;
    }
}


