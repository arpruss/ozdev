main()
{
    unsigned size;
    unsigned i,j;
    static char s[244];
    ozsinglepage();
    size=ozgetauxsize();
    sprintf(s,"Size:  %d",size);
    ozputs(0,0,s);
    for(i=1;i<size-1;i+=2)
    {
        ozwriteauxword(i,i);
        ozputs(20,20,s);
    }
    for(i=1;i<size-1;i+=2)
        if(i!=(j=ozreadauxword(i)))
        {
            sprintf(s,"Error at %x: %x",i,j);
            ozputs(10,10,s);
            ozgetch();
        }
}
