#include <stdlib.h>
#include <mem.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>

/* must be compiled in tiny model under Turbo C */

void interrupt (*old21)();
void interrupt my21();

char blanks[512];

#ifdef DEBUG
long number_called=0,number_fixed=0;
long number_i21=0;

char *toascii(long n)
{
  static char string[31];
  char *p;
  int neg;
  neg=0;
  if(n<0) neg=1;
  if(n==0) return "0";
  p=string+29;
  p[1]=0;
  while(n)
  {
    *p='0'+(n%10);
    n/=10;
    p--;
  }
  if(neg) *p--='-';
  return p+1;
}

char buf0[100];
#endif

int err;
unsigned long _lseek(unsigned handle,long pos,char whence)
{
  static unsigned long output;
  asm mov al,[whence]
  asm mov ah,42h
  asm mov bx,[handle]
  asm mov dx,word ptr [pos]
  asm mov cx,word ptr [pos+2]
  asm pushf
  asm call cs:[old21]
  asm mov word ptr [output],ax
  asm mov word ptr [output+2],dx
  asm mov ax,0
  asm jnc NoError
  asm inc ax
NoError:
  asm mov cs:[err],ax
  return output;
}

void interrupt my21(unsigned ibp,unsigned idi,
	unsigned isi,unsigned ids,unsigned ies,
	unsigned idx,unsigned icx,unsigned ibx,
	unsigned iax,unsigned iip,unsigned ics,unsigned iflags)
{
  unsigned long newOffset,eofOffset,seekOffset;
  unsigned long pos,pos2;
  unsigned long i;
  int handle;
  int nOrigin;
  unsigned func;
#ifdef DEBUG
  number_i21++;
#endif
  func=iax>>8;
  if(func==0x42)
  {
#ifdef DEBUG
     number_called++;
#endif
     pos=_lseek(ibx,0,SEEK_CUR);
     eofOffset=_lseek(ibx,0,SEEK_END);
     pos2=_lseek(ibx,pos,SEEK_SET);
     seekOffset=(((unsigned long)icx)<<16) | idx;
     newOffset=_lseek(ibx,seekOffset,iax&0xFF);
     iax=newOffset;
     idx=newOffset>>16;
     if(err)
	iflags|=1;
     else
	iflags&=~1;
     if(pos2==pos && !err && newOffset>eofOffset && ibx>4 && (newOffset-eofOffset)<100000)
     {
#ifdef DEBUG
	number_fixed++;
#endif
	_lseek(ibx,eofOffset,SEEK_SET);
	for(i=newOffset-eofOffset;i>=512;i-=512) _write(ibx,blanks,512);
	if(i>0) _write(ibx,blanks,i);
     }
     return;
  }
  asm push	si
  asm push	di
  asm push	ds
  asm push	es

  asm mov	ax,[iax]
  asm mov	bx,[ibx]
  asm mov	cx,[icx]
  asm mov	dx,[idx]
  asm mov	di,[idi]
  asm mov	si,[isi]
  asm mov	es,[ies]
  asm mov	ds,[ids]
  asm pushf
  asm call	cs:[old21]
  asm pushf
  asm mov	[iax],ax
  asm pop	ax
  asm mov	[iflags],ax
  asm mov	[ibx],bx
  asm mov	[icx],cx
  asm mov	[idx],dx
  asm mov	[idi],di
  asm mov	[isi],si
  asm mov	[ies],es
  asm mov	[ids],ds

  asm pop	es
  asm pop	ds
  asm pop       di
  asm pop	si

}

main(int argc, char **argv)
{
  int r;
  int l;
  int disable=0;
  char *p;
  l=strlen(argv[0]);
  if(l<9 || stricmp(argv[0]+l-9,"hlin1.exe"))
  {
    fprintf(stderr,"This executable must be named hlin1.exe.\n");
    return 99;
  }
  argv[0][l-9+4]='k';
  if(!strcmp(getenv("OS"),"Windows_NT")) disable=1;
  if(!disable)
  {
     memset(blanks,0,512);
     old21=getvect(0x21);
     setvect(0x21,my21);
  }
  r=spawnv(P_WAIT,argv[0],argv);
  if(!disable) setvect(0x21,old21);
  if(r==-1)
  {
    fprintf(stderr,"Error executing %s.\n",argv[0]);
    return 98;
  }
#ifdef DEBUG
  printf("%ld 0x21 calls;  %ld lseek calls;  %ld fixed\n",number_i21,number_called,number_fixed);
#endif
  return r;
}
