#include <string.h>
#include <oz.h>
#include "edit.h"

byte num_recnums;
#define MAX_RECNUMS 10
extern unsigned recnums[MAX_RECNUMS];
byte issecret;
byte searchmode;
static byte error;

extern char _ozclusterbuffer[];
#define clusterdata (2+_ozclusterbuffer)


char sig[TITLE_LENGTH+1];
extern byte memo[HEADER+MAX_MEMO_LENGTH+1]; /* =
"MEMO\xFF\xFF\xD0\x0D\x00\x16\x00\x00\x00"
"YYYYMMDD\x00MemoTitleHere       \x00"; */

/* char encoding[]=
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz_=";
*/

byte encoding(byte _x)
{
    static byte x;
    x=_x;
    if(x<26) return x+'A';
    else if(x<(byte)(26+10)) return x+(byte)('0'-26);
    else if(x<(byte)(26+10+26)) return x+(byte)('a'-26-10);
    else if(x==(byte)(26+10+26)) return '_';
    else return '=';
}

char info_message[]=
"\r"
/* "Special MemoEdit memo. Don't edit!"; */
"MemoEdit:Don't edit!";

/*
void encode_word(char *out, unsigned word)
{
    register char *p=out;
    p[0]=encoding((byte)(word>>10));
    p[1]=encoding((0x1f&(byte)(word>>5)));
    p[2]=encoding((0x1f&(byte)word));
}

unsigned decode_word(char *in)
{
    register char *p=in;
    return (p[0]-33)|( (p[1]-33)<<6 ) | ( (p[2]-33)<<12 );
}
*/

void ascii_encode8(char *out, byte *in)
{
    static char *p;
    register unsigned *q=(unsigned*)in;
    static unsigned i;
    p=out;
    for(i=0;i<4;i++)
    {
        p[0]=encoding((byte)((*q)>>10));
        p[1]=encoding((0x1f&(byte)((*q)>>5)));
        p[2]=encoding((0x1f&(byte)(*q)));
        p+=3;
        q++;
    }
    *p=0;
}

#define generate_signature(t,parts) \
{ \
    strcpy(sig,special_header); \
    ascii_encode8(sig+SPECIAL_LENGTH,(byte*)&(t)); \
    sig[SPECIAL_LENGTH+12]=encoding( (byte)(getR())&0x3f ); \
    sig[SPECIAL_LENGTH+13]=encoding((byte)(parts)); \
    sig[SPECIAL_LENGTH+14]=0; \
}

void utoa0_n(char *s, unsigned val, byte n)
{
    register char *p=s+n;
    *p=0;
    while(p>s)
    {
        p--;
        *p=val%10+'0';
        val/=10;
    }
}

void getdatestring()
{
#define DATE ((char*)memo+DATE_OFFSET)
    static unsigned year;
    static unsigned month,day;
    day=ozday();
    month=ozmonth();
    year=ozyear();
    if((byte)day!=(byte)ozday())
    {
        day=ozday();
        month=ozmonth();
        year=ozyear();
    }
    utoa0_n(DATE,year,4);
    utoa0_n(DATE+4,month,2);
    utoa0_n(DATE+6,day,2);
#undef DATE
}

void do_wait(void)
{
    /* ensure two files are never saved at the same time */
    while(oztime_fake()==head0.start_time) ozintwait();
}

int save_file(void)
{
    static byte i;
    static byte num_parts;
    static unsigned offset,length,part_length;
    for(i=0;i<num_recnums;i++)
    {
        ozunlink(TYPENUM_MEMO,recnums[i]);
    }
    getdatestring();
    memcpy((char*)memo+TITLE_OFFSET,title,TITLE_LENGTH);
    memo[TITLE_OFFSET+TITLE_LENGTH]=0;
    for(i=0;i<TITLE_LENGTH;i++)
        if(memo[TITLE_OFFSET+i]==0)
            for(;i<TITLE_LENGTH;i++)
               memo[TITLE_OFFSET+i]=' ';
    if(issecret) memo[6]=0x50; else memo[6]=0xD0;
    if(file_length<=MAX_MEMO_LENGTH)
    {
        ozreadauxmem(0,memo+HEADER,file_length);
        memo[HEADER+file_length]=0;
        if(!ozwritefile(memo,HEADER+file_length+1,TYPENUM_MEMO))
        {
            goto_recnum=ozgetlastrecnum();
            return 0;
        }
        return -1;
    }
    num_parts=(file_length+(unsigned)(MAX_MEMO_LENGTH-1))/MAX_MEMO_LENGTH;
    head0.x0=ozcrc16auxmem(0,256);
    head0.x1=ozcrc16auxmem(file_length-256,256);
      /* this had better behave well even if file_length<256 */
    generate_signature(head0,num_parts);
    strcpy((char*)memo+TITLE_OFFSET+TITLE_LENGTH+1,sig);
    strcpy((char*)memo+TITLE_OFFSET+TITLE_LENGTH+1+TITLE_LENGTH,info_message);
    if(ozwritefile(memo,HEADER+TITLE_LENGTH+1+sizeof(info_message),TYPENUM_MEMO))
       return -1;
    goto_recnum=ozgetlastrecnum();
    length=file_length;
    offset=i=0;
    strcpy((char*)memo+DATE_OFFSET,special_date);
    while(length>0)
    {
        if(length>=MAX_MEMO_LENGTH) part_length=MAX_MEMO_LENGTH;
          else part_length=length;
        sig[SPECIAL_LENGTH+13]=encoding(i);
        strcpy((char*)memo+TITLE_OFFSET,sig);
        ozreadauxmem(offset,memo+HEADER,part_length);
        memo[HEADER+part_length]=0;
        if(ozwritefile(memo,HEADER+part_length+1,TYPENUM_MEMO))
        {
            do_wait();
            return -1;
        }
        length-=part_length;
        offset+=part_length;
        i++;
    }
    do_wait();
    return 0;
}

int write_file(void)
{
    static byte i,c,last;
    static int retval;
    if(file_length==0)
    {
        ozwriteauxbyte(0,' ');
        file_length=1;
    }
    else
        export_file();
    for(last=i=0;i<TITLE_LENGTH && i<file_length;i++,last++)
    {
        if((c=ozreadauxbyte(i))=='\r')
        {
            last++;
            break;
        }
        else title[i]=c;
    }
    for(;i<TITLE_LENGTH;i++) title[i]=' ';
    if(last==TITLE_LENGTH && TITLE_LENGTH<file_length
        && ozreadauxbyte(TITLE_LENGTH)=='\r') last++;
    title[TITLE_LENGTH]=0;
    c=title[4];
    title[4]=0;
    if(!stricmp(title,"zzzz") && (c=='y' || c=='z' || c=='Y'
        || c=='Z'))
    {
        title[3]='_';
    }
    title[4]=c;
    delete(0,last);
    file_length-=last;
    if(file_length==0)
    {
        ozwriteauxbyte(0,' ');
        file_length=1;
    }
    ozsetpowerkeyhandling(0);
    retval=save_file();
    ozsetpowerkeyhandling(1);
}

void readtoend(unsigned loc)
{
    static unsigned len;
    if(0xFFFF==loc) return;
    do
    {
        ozloadcluster(loc);
        len=strlen(clusterdata);
        if(file_length+len>16384)
        {
            len=16384-file_length;
            error=1;
        }
        if(len==0) return;
        ozwriteauxmem(file_length,clusterdata,len);
        file_length+=len;
    } while((loc=ozinclocation(loc))!=0xFFFF);
}

void error_warn(void)
{
   beep();
   warn("Error in file.",anykey_message);
}

void do_strip_file(void)
{
    if(!searchmode && import_file()) /* don't import if not searchmode */
    {
        beep();
        warn("Special characters lost.",anykey_message);
    }
}

void dodel(byte k)
{
    static byte i;
    for(i=0;i<k;i++)
      ozunlink(TYPENUM_MEMO,recnums[i]);
}

int load_or_delete_memo(unsigned memo_num,byte delete)
{
    static byte found;
    static unsigned loc,len;
    static unsigned n;
    static unsigned i,rn;
    static unsigned recnum;
    static char i1;
    static byte j1,j,k,tlen;
    issecret=error=0;
    if(quick_index)
        recnum=ozreadauxword2(memo_num);
    else
    {
        n=0;
        for(i=0;i<index_count;i++)
        {
            if(n+index_lengths[i]>memo_num)
            {
               j1=memo_num-n;
               break;
            }
            n+=index_lengths[i];
        }
        if(i==index_count) return -1;
        ozopenfile(index_locs[i]);
        for(j=j1;j>0;j--)
        {
            ozgetw();
            ozreadfilebyte();
        }
        recnum=ozgetw();
    }
    loc=ozfindrecnum(recnum);
    if(!loc) return -1;
    recnums[0]=recnum;
    num_recnums=1;
    ozloadcluster(loc);
    memcpy(sig,clusterdata+MEMO_DATA_OFFSET,TITLE_LENGTH-1);
    memcpy(title,clusterdata+TITLE_OFFSET,TITLE_LENGTH);
    issecret=!(clusterdata[6]&0x80);
    if(issecret && islocked && !delete)
    {
        if(!searchmode)
            warn("Secret memo: Unlock Wizard first.",anykey_message);
        return -1;
    }
    for(i1=TITLE_LENGTH-1;i1>=0 && title[i1]==' ';i1--);
    tlen=i1+1;
    file_length=tlen;
    if(strncmp(sig,special_header,SPECIAL_LENGTH))
    {
        if(delete)
        {
            ozunlink(TYPENUM_MEMO,recnum);
            return 0;
        }
        ozwriteauxmem(0,title,tlen);
        quick_index=0;
        ozwriteauxbyte(tlen,'\r');
        file_length++;
        ozwriteauxmem(file_length,clusterdata+MEMO_DATA_OFFSET,62-MEMO_DATA_OFFSET);
        file_length+=strlen(clusterdata+MEMO_DATA_OFFSET);
        readtoend(ozinclocation(loc));
        do_strip_file();
        return 0;
    }
    ozloadcluster(ozinclocation(loc));
    sig[TITLE_LENGTH-1]=*clusterdata;
    sig[TITLE_LENGTH]=0;
    num_recnums=1+sig[SPECIAL_LENGTH+13]-'A';
    if(num_recnums>MAX_RECNUMS)
    {
        num_recnums=MAX_RECNUMS;
        error=1;
    }
    if(quick_index)
    {
       for(n=start_extended;n<total_memos;n++)
       {
         loc=ozfindrecnum(ozreadauxword2(n));
         ozloadcluster(loc);
         if(!strncmp(sig,clusterdata+TITLE_OFFSET,SPECIAL_LENGTH+12)) break;
       }
       if(n==total_memos)
       {
          if(delete)
          {
             dodel(1);
             error_warn();
             return 0;
          }
          else return -1;
       }
       for(k=j=1;j<num_recnums && n<total_memos;j++,n++)
       {
          loc=ozfindrecnum(recnums[k]=ozreadauxword2(n));
          ozloadcluster(loc);
          if(!strncmp(sig,clusterdata+TITLE_OFFSET,SPECIAL_LENGTH+12))
            k++;
       }
       if(k<num_recnums)
       {
          if(delete)error_warn();
          error=1;
          num_recnums=k;
       }
    }
    else
    {
        n=0;
        for(i=0;i<index_count;i++)
        {
            if(n+index_lengths[i]>start_extended)
            {
               j1=start_extended-n;
               break;
            }
            n+=index_lengths[i];
        }
        if(i==index_count)
        {
            if(delete)
            {
                dodel(1);
                error_warn();
                return;
            }
            else
                return -1;
        }
        found=k=0;
        for(;i<index_count;i++)
        {
            ozopenfile(index_locs[i]);
            for(j=0;j<j1;j++)
            {
                ozreadfilebyte();
                ozreadfilebyte();
                ozreadfilebyte();
            }
            for(j=j1;j<index_lengths[i];j++)
            {
                ozloadcluster(ozfindrecnum(rn=ozgetw()));
                ozreadfilebyte();
                if(!found &&
                    !strncmp(sig,clusterdata+TITLE_OFFSET,SPECIAL_LENGTH+12))
                {
                    found=1;
                    k=1;
                }
                if(found)
                {
                    recnums[k++]=rn;
                    if(k==num_recnums) goto FOUND;
                }
            }
            j1=0;
        }
        if(delete)
        {
            dodel(k);
            error_warn();
            return 0;
        }
        if(!found) return -1;
        if(delete)error_warn();
        error=1;
        num_recnums=k;
        FOUND:;
    }
    if(delete)
    {
        dodel(num_recnums);
        return 0;
    }
    quick_index=0;
    ozwriteauxmem(0,title,tlen);
    for(i=1;i<num_recnums;i++)
    {
        ozloadcluster(loc=ozfindrecnum(recnums[i]));
        len=strlen(clusterdata+MEMO_DATA_OFFSET);
        if(file_length+len>16384)
          len=16384-file_length;
        if(!len) break;
        ozwriteauxmem(file_length,clusterdata+MEMO_DATA_OFFSET,len);
        if(clusterdata[TITLE_OFFSET+SPECIAL_LENGTH+13]!=encoding(i-1)) error=1;
        file_length+=len;
        if(file_length>=16384) break;
        readtoend(ozinclocation(loc));
        if(file_length>=16384) break;
    }
    if(file_length>16384) file_length=16384;
    if(file_length<16384)
    {
        insert(tlen,1);
        file_length++;
        ozwriteauxbyte(tlen,'\r');
    }
    do_strip_file();
    if(error && !searchmode) error_warn();
    return 0;
}

