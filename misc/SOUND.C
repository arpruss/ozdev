void mozsound(unsigned s)
{
    char buf[12];
    sprintf(buf,"%d",s);
    ozputs(10,10,buf);
    ozsound(s);
}

void wait(unsigned i)
{
    while(i) i--;
}

main()
{
    ozclick(0);
    mozsound(0);
    wait(65535);
    mozsound(1);
    wait(65535);
    mozsound(2);
    wait(65535);
    mozsound(3);
    wait(65535);
    mozsound(4);
    wait(65535);
    ozquiet();
}
