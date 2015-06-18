main()
{
    ozputs(10,10,"Page 1");
    ozcopypage(1,0);
    ozcls();
    ozgetch();
    ozsetdisplaypage(1);
    ozgetch();
    ozcopypage(0,1);
    ozsetdisplaypage(0);
    ozgetch();
}
