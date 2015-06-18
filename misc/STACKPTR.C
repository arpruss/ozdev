main()
{
    char buf[20];
    sprintf(buf,"%x",(unsigned)buf);
    ozputs(10,10,buf);
    ozgetch();
}
