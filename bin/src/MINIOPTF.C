/* optimizes floating point constants in output of Hi-Tech C compiler */
/* replaces:
    psect data
    e100: deff N.N
    psect text
    ld hl,(e100+2)
    ld de,(e100)
   with:
    psect data
    psect text
    ld hl,NNNN
    ld de,NNNN
   Also, changes other deff lines to two defw lines, to get around assembler
   bug and hopefully get a bit more precision by doing the decimal-to-binary
   conversion in double precision on the platform on which minioptf is running.
 */


#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void toz80(unsigned char *out, double x)
{
    double m;
    int e;
    int i;
    unsigned char z;
    int neg=0;
    long m2;
    m=frexp(x,&e);
    m2=floor(ldexp(m,3*8));
    if(m2<0)
    {
        m2=-m2;
        neg=1;
    }
    for(z=i=0;i<3;i++)
    {
        z|=(out[i]=m2&0xFF);
        m2>>=8;
    }
    if(!z) out[3]=0;
      else out[3]=(neg<<7) | ((e+0x40) & 0x7F);
}

int scanlabel(char *line,char *label,unsigned char *out)
{
/* search for e123:\tdeff x */
/*                01 234567 */
    char *z;
    char *p;
    int len;
    if(line[0]!='e') return 0;
    z=strchr(line,':');
    if(z==NULL || z==line+1) return 0;
    for(p=line+1;p<z;p++)
        if(!isdigit(*p)) return 0;
    if(z[1]!='\t' || strncmp(z+2,"deff ",5)) return 0;
    toz80(out,atof(z+7));
    strncpy(label,line,len=z-line);
    label[len]=0;
    return len;
}

main()
{
    char line[1024];
    int i,l;
    int data;
    int text;
    int allocainfunction;
    int hadcallncsv;
    int labellen=0;
    char label[25];
    unsigned offset;
    unsigned value;
    unsigned char z80float[4];
    char *s;
    char stacksizename[32];
    int ssn_length;
    int secondword;
    setvbuf(stdin,NULL,_IOFBF,24*1024);
    setvbuf(stdout,NULL,_IOFBF,24*1024);
    text=0;
    while(NULL!=gets(line))
    {
        if(!strncmp(line,"deff\t",5))
        {
            toz80(z80float,atof(line+5));
            printf("defw %u\ndefw %u\n",
             z80float[0]+z80float[1]*(unsigned int)0x100,
             z80float[2]+z80float[3]*(unsigned int)0x100);
            continue;
        }
        if(!strncmp(line,"psect\t",6))
        {
            data=!strncmp(line+6,"data",4);
            text=!strncmp(line+6,"text",4);
            puts(line);
            continue;
        }
        if(l=scanlabel(line,label,z80float))
        {
            labellen=l;
            continue;
        }
        if(labellen && text && !strncmp(line,"ld\t",3)
            && line[5]==',' && line[6]=='(' && !strncmp(line+7,label,labellen)
            && (line[7+labellen]==')'
                 || (!strncmp(line+7+labellen,"+2)",3))))
        {
        /* ld xx,(label[+2]) */
        /* 01234567 */
            if(line[7+labellen]==')') secondword=0;
                else secondword=2;
            value=z80float[secondword] + z80float[secondword+1]*(unsigned int)0x100;
            printf("ld\t%c%c,%u\n",line[3],line[4],value);
        }
        else puts(line);
    }
   return 0;
}
