#include <stdio.h>

main(int argc, char **argv)
{
   FILE *in, *out;
   char name[256];
   int c;
   unsigned page=0x408;
   if(argc!=3 && argc!=4)
   {
     fprintf(stderr,"loadlib binfile libfile [page]\n");
     return 4;
   }
   if(argc==4) sscanf(argv[3],"%x",&page);
   strcpy(name,argv[1]);
   strcat(name,".bin");
   in=fopen(name,"rb");
   if(in==NULL)
   {
     fprintf(stderr,"Cannot open %s.\n",name);
     return 5;
   }
   strcpy(name,argv[1]);
   strcat(name,".out");
   in=fopen(name,"wb");
   if(in==NULL)
   {
     fprintf(stderr,"Cannot open %s
