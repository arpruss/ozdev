#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <oz.h>
#include "edit.h"


/* #define DEMO /**/
/* #define EXPORT /**/
/* #define MAIL /**/
/* #define EXPORT /**/

#define KEY_PTR ((unsigned long*)key)

#ifndef MAIL
static byte key[16];
static byte encrypt_pos=0;
static byte block[8];
static byte oblock[8];
#endif

void __ozfilemysrand();
int __ozfilemyrand();


#define BYTEn(x,n)  ( (byte)( ((byte*)&(x))[n] ) )
#define BYTE1(x) BYTEn(x,1)
#define BYTE2(x) BYTEn(x,2)

#define MAX_ACTUAL_PASSWORD 22
#ifdef EXPORT
#define MAX_DO_PASSWORD 10
#define MAX_PASSWORD 10
#else
#define MAX_DO_PASSWORD 28
#define MAX_PASSWORD 28
#endif

#define RANDOMIZED

byte randhbit(void);
byte randlbit(void);

char password[MAX_PASSWORD+1];
char filename[MAX_FILENAMELEN+1];
char errwrite[]="Error writing!";
char error_reading[]="Error reading!";
char newencr[8]="NewEncrp";

char sstring[]="Search string: ";

static byte error=0;

extern char search_string[];

void beep(void)
{
    ozinitsound();
    ozsound(30);
    ozdelay64hz(4);
    ozquiet();
}

#ifndef MAIL
char directory[]="secure/";

byte get_password(int x, int y, char *_password)
{
#define ASTERISK_SIZE 6
    register char *password=_password;
    static byte pos;
    static byte x1;
    static unsigned c;
    pos=0;
    x1=x;
    *password=0;
    ozfont(FONT_PC_NORMAL);
    while(1) switch(c=getch())
    {
        case 27:
        case KEY_UPPER_ESC:
          return 1;
        case KEY_LOWER_ENTER:
        case KEY_UPPER_ENTER:
        case '\n':
#ifdef DEMO
          password[1]=0;
#endif
          return 0;
        case 8:
          if(!pos) break;
          password[--pos]=0;
          x1-=ASTERISK_SIZE;
          ozputs(x1,y,"  ");
          break;
        default:
          if(c>127 || (!isalnum(c) && c!='.' && c!=' ') ||
          pos>=MAX_DO_PASSWORD) break;
          ozputch(x1,y,'*');
          password[pos++]=c;
          x1+=ASTERISK_SIZE;
          password[pos]=0;
    }
}

unsigned long get6bits(char k)
{
    if(isdigit(k)) return k-'0';
    if(k==' ') return 11;
    if(k=='.') return 12;
    if(k>='a' && k<='z') return 13+k-'a';
    return 13+26+k-'A';
}

void make_key(char *password, byte *key)
{
    static char p_final[MAX_ACTUAL_PASSWORD+1];
    static byte i,l;
    static unsigned long encoded;
    static unsigned last_byte;
    register char *p;
    strncpy(p_final,password,MAX_ACTUAL_PASSWORD);
    p_final[MAX_ACTUAL_PASSWORD]=0;
    if((l=strlen(password))<MAX_ACTUAL_PASSWORD)
    {
        p=password;
        for(i=l;i<MAX_ACTUAL_PASSWORD;i++)
        {
            if(p>=password+l) p=password;
            p_final[i]=*p;
            p++;
        }
    }
/*    ozputs0(0,p_final);
    ozgetch();
*/
    p_final[MAX_ACTUAL_PASSWORD]=0;
    for(i=0;i<5;i++)
    {
        p=p_final+4*i;
        encoded = get6bits(p[0]) | get6bits(p[1])<<6
                | get6bits(p[2])<<12
                | get6bits(p[3])<<18;
        p=(char*)(key+3*i);
        p[0]=(byte)encoded;
        p[1]=BYTE1(encoded);
        p[2]=BYTE2(encoded);
    }
    key[15]=get6bits(p_final[20]) | (last_byte=get6bits(p_final[21])<<6);
    last_byte=last_byte>>6;
    /* there are two bits left in the last_byte */
    for(i=0;i<16;i++) key[i]^=last_byte;
    if(l>MAX_ACTUAL_PASSWORD)
    {
        for(i=0;password[i+MAX_ACTUAL_PASSWORD];i++)
          key[i]^=get6bits(password[i]);
    }
}

/* TEA code */

#if 0
void encipherold(unsigned long *v,unsigned long *w,
   unsigned long * k)
   {
#define delta 0x9E3779B9l
       static unsigned long y,z,sum,a,b,c,d;
       static byte n;
       y=v[0];
       z=v[1];
       sum=0;
       a=k[0];
       b=k[1];
       c=k[2];
       d=k[3];
       n=32;

   while(n-->0)
         {
           sum += delta;
           y += (z << 4)+a ^ z+sum ^ (z >> 5)+b;
           z += (y << 4)+c ^ y+sum ^ (y >> 5)+d;
         }

   w[0]=y; w[1]=z;
#undef delta
}
#endif

void encipher(unsigned long *v,unsigned long *w)
   {
#define delta 0x9E3779B9l
       static unsigned long y,z,sum;
       static byte n;
       y=v[0];
       z=v[1];
       sum=0;
       n=32;

   while(n-->0)
         {

          y += (z << 4 ^ z >> 5) + z ^ sum + KEY_PTR[((byte)sum)&3];
          sum += delta;
          z += (y << 4 ^ y >> 5) + y ^ sum + KEY_PTR[BYTE1(sum)>>3 & 3];
         }

   w[0]=y; w[1]=z;
#undef delta
}

void (*decipher)(unsigned long *,unsigned long *);

void deciphernew(unsigned long *v,unsigned long *w)
   {
#define delta 0x9E3779B9l
       static unsigned long y,z,sum;
       static byte n;
       y=v[0];
       z=v[1];
       sum=0xC6EF3720l;
       n=32;

   /* sum = delta<<5, in general sum = delta * n */

   while(n-->0)
         {
          z -= (y << 4 ^ y >> 5) + y ^ sum + KEY_PTR[BYTE1(sum)>>3 & 3];
          sum -= delta;
          y -= (z << 4 ^ z >> 5) + z ^ sum + KEY_PTR[((byte)sum)&3];
         }

   w[0]=y; w[1]=z;
}

void decipherold(unsigned long *v,unsigned long *w)
   {
       static unsigned long y,z,sum;
       static byte n;
       y=v[0];
       z=v[1];
       sum=0xC6EF3720l;
       n=32;

   /* sum = delta<<5, in general sum = delta * n */

   while(n-->0)
         {
            z -= (y << 4)+KEY_PTR[2] ^ y+sum ^ (y >> 5)+KEY_PTR[3];
            y -= (z << 4)+KEY_PTR[0] ^ z+sum ^ (z >> 5)+KEY_PTR[1];
            sum -= delta;
         }

   w[0]=y; w[1]=z;
}

void encrypt_write(byte b)
{
    if(error) return;
#ifdef RANDOMIZED
    b^=randhbit();
#endif
    block[encrypt_pos++]=b;
    if(encrypt_pos==8)
    {
        encrypt_pos=0;
        encipher((void*)block,(void*)oblock);
/*
memcpy(oblock,block,8);
*/
        /* rotate key */
        if(write1(oblock,8)<8)
        {
            warn(errwrite,anykey_message);
            error=1;
            return;
        }
        encrypt_pos=0;
        return;
    }
}

void flush_write_block()
{
    while(encrypt_pos) encrypt_write(
#ifdef RANDOMIZED
    ((unsigned)__ozfilemyrand())&0xFF
#else
    0
#endif
    );
}

byte decrypt_read_block(byte *block)
{
    if(read1(oblock,8)<8)
        return 0;
    decipher((void*)oblock,(void*)block);
    return 8;
}

void prepare(void)
{
    static unsigned offset;
    for(offset=0;offset<file_length;offset++)
        ozwriteauxbyte(offset,randhbit()|ozreadauxbyte(offset));
}
#else
char directory[]="o/";
#endif

byte wrote;

int write_file(void)
{
    static unsigned pos,size;
#ifndef MAIL
    static char p1[MAX_PASSWORD+1],p2[MAX_PASSWORD+1];
#endif
    static byte same_pass,x;
    static unsigned i;
    ozfont(FONT_PC_NORMAL);
#ifndef FIXED
    line_height=10;
    line_height_bytes=10*30;
#endif
    wrote=0;
    ozcls();
#ifndef MAIL
    if(*password==0)
    {
        if(warn("Save as clear text?",yn_message)==KEY_LOWER_ENTER)
            same_pass=1;
        else same_pass=0;
/*        ozputs0(10,"WARNING: If you save as clear text, then");
        ozputs0(20,"even after you delete the file, it will be");
        ozputs0(30,"recoverable for a long time unless you run Wiper!");
*/
    }
    else
    {
        if(warn("Keep same password?",yn_message)==KEY_LOWER_ENTER)
           same_pass=1;
        else
           same_pass=0;

    }
    ozcls();
    if(!same_pass)
    {
        ozputs0(50,"Passwords may contain A-Z, a-z, 0-9, period and space.");
        do
        {
           clear_rest(6);
           x=ozputs0(60,"New password? ");
           if(get_password(x,60,p1)) return;
           x=ozputs0(70,"Confirm? ");
           if(get_password(x,70,p2)) return;
        } while(strcmp(p1,p2));
#ifdef DEMO
        p1[1]=0;
#endif
        strcpy(password,p1);
#ifdef DEMO
        password[1]=0;
#endif
    }
#endif /* MAIL */
    ozsetpowerkeyhandling(0);
    if(open1(filename,O_WRONLY)<0)
    {
        warn(errwrite,anykey_message);
        ozsetpowerkeyhandling(1);
        return;
    }
    pos=0;
    clear_wrap_to_end(0);
#ifndef MAIL
    if(!*password)
#endif
    {
        ozcls();
        ozputs0(0,"Saving clear text...");
        while(pos<file_length)
        {
        if(pos+LINE_BUF_LEN<=file_length) size=LINE_BUF_LEN;
          else size=file_length-pos;
        ozreadauxmem(pos,line_buf,size);
        if(write1(line_buf,size)<size)
        {
            warn(errwrite,anykey_message);
            close1();
            ozsetpowerkeyhandling(1);
            return;
        }
        pos+=size;
        }
        wrote=1;
        close1();
        ozsetpowerkeyhandling(1);
        return;
    }
#ifndef MAIL
    ozcls();
    ozputs0(0,"Encrypting...");
    write1(newencr,8);
    make_key(password,key);
    encrypt_pos=0;
    encrypt_write( ((byte)file_length & (byte)7)
#ifdef RANDOMIZED
    | ( __ozfilemyrand()&(byte)(0xff-7) )
#endif
    );
    flush_write_block();
    prepare();
    for(i=0;i<file_length;i++) encrypt_write(ozreadauxbyte(i));
    flush_write_block();
    close1();
    ozsetpowerkeyhandling(1);
    if(!error) wrote=1;
#endif
}


void read_file(void)
{
    static int r;
    static byte x,i,b;
    static byte size8;
    file_length=0;
    if(open1(filename,O_RDONLY)<0)
    {
        warn(error_reading,anykey_message);
        exit(0);
    }
#ifndef MAIL
    clearbottom();
    x=ozputs0(71,"Password: ");
    get_password(x,71,password);
    if(*password==0)
#endif
    {
    ozcls();
    ozputs0(0,"Reading...");
    do
    {
        r=read1(line_buf,LINE_BUF_LEN);
        if(file_length+r>MAX_FILE_LENGTH)
        {
            ozcls();
            if(warn("Too long! Truncate?",yn_message)!=KEY_LOWER_ENTER)
                exit(0);
            r=MAX_FILE_LENGTH-file_length;
        }
        if(r>0)
        {
            for(i=0;i<r;i++)
            {
                 b=line_buf[i];
                 b&=0x7f;
                 line_buf[i]=b;
            }
            ozwriteauxmem(file_length,line_buf,r);
            file_length+=r;
        }
    } while(r==LINE_BUF_LEN && file_length < MAX_FILE_LENGTH);
    close1();
    return;
   }
#ifndef MAIL
   ozcls();
   ozputs0(0,"Decrypting...");
   make_key(password,key);
   encrypt_pos=0;
   if(read1(oblock,8)<8)
   {
      warn(error_reading,anykey_message);
      exit(0);
   }
   if(strncmp((char*)oblock,newencr,8))
   {
      decipher=decipherold;
      decipherold((void*)oblock,(void*)block);
   }
   else
   {
      decipher=deciphernew;
      decrypt_read_block(block);
   }
   size8=block[0]&7;
   if(size8==0) size8=8;
   file_length=0;
   while(decrypt_read_block( block )==8 && file_length<MAX_FILE_LENGTH)
   {
      for(i=0;i<8;i++)
         block[i]&=0x7f;
      ozwriteauxmem(file_length,block,8);
      file_length+=8;
   }
   file_length-=(8-size8);
   close1();
#endif
}

main()
{
/*    ozinitprofiler(); */

    installcursor();
    preparehelp();
    ozsinglepage();
#ifndef MAIL
    ozinstallmemorywiper();
#endif
    ozrestorekeysettings();
    _ozautoofftime=0;
    load_clip_permanent();
    restore_clipboard();

    switch(ozscancustomfonts())
    {
        case 0: NUM_FONTS=2; break;
        case 1: NUM_FONTS=3; font2=FONT_CUSTOM0; break;
        case 2: NUM_FONTS=3; font2=FONT_CUSTOM1; break;
        case 3: NUM_FONTS=4; font2=FONT_CUSTOM0; font3=FONT_CUSTOM1; break;
    }
    switch(ozgetappfont())
    {
        case 0: font=1; break;
        case 1: font=0; break;
        case FONT_CUSTOM0:
                font=2;
                break;
        case FONT_CUSTOM1:
                if(NUM_FONTS==4) font=3;
                    else font=2;
                break;
    }
    if(font>=NUM_FONTS) font=0;


#ifdef RANDOMIZED
    __ozfilemysrand();
#endif
    menu();
}

