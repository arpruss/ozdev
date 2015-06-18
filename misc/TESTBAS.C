main()
{
    extern unsigned *_ozinitialsp;
    static char buf[244];
    ozcls();
    sprintf(buf,"%x %x %x",_ozinitialsp[0],_ozinitialsp[1],_ozinitialsp[2]);
    ozputs(10,10,buf);
    _ozinitialsp[1]-=9;
    sprintf(buf,"%x %x %x",_ozinitialsp[0],_ozinitialsp[1],_ozinitialsp[2]);
    ozputs(20,20,buf);
    ozgetch();
}


