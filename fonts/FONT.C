main()
{
    int w;
    int c;
    puts("_ xygTW|{[&^_@#`~ sample");
    putchar('\n');
    w=0;
    for(c=32;c<127;c++,w++)
    {
        if(w>12)
        {
            putchar('\n');
            w=0;
        }
        printf("_%c_ ",c);
    }
}



