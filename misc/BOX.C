main()
{
    int i;
    int x;
    ozbox(10,20,30,40);
    ozputs(40,40,"Hello world!");
    for(i=0;i<10;i++) ozdisplayorbyte(i*2,0xFE);
    ozputch(50,50,'.');
    x=ozputch(ozputch(50,-1,'.'),55,'!');
    ozputch(x,60,'@');
}
