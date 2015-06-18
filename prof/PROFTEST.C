f(){
    unsigned int x;
    for(x=0;x<1000;x++);
    }

g(){unsigned int x;
    for(x=0;x<2000;x++);}

main()
{
    ozinitprofiler();
TOP:
    f();
    g();
    goto TOP;
}
