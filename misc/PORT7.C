close(){}

main()
{
    char buf[15];
    unsigned x;
    x=ozportin(7);
    sprintf(buf,"%x",x);
    ozputs(10,10,buf);
    ozgetch();
}
