close(){}

main()
{
    int i,j;
    unsigned long sum;
    long *p=(long*)0xa000;
    char string[30];
    ozcls();
    for(i=0;i<16;i++)
    {
      ozportout(3,i);
      ozportout(4,4);
      sum=0;
      for(j=0;j<2048;j++)
         sum+=p[j];
      sprintf(string,"%d - x%08lx",i,sum);
      ozputs((i/8)*120,(i%8)*10,string);
    }
    ozgetch();
}

