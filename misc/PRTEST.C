char s00[16000]="Hello!";
char s0[]="String 0";
char s11[4000]="Hi!";
char s1[]="String 1";
char s2[]="String 2";

void termputs(char *s)
{
    while(*s)
    {
        termputch(*s);
        s++;
    }
}

main()
{
    termputs(s0);
    termputs("\r\n");
    termputs(s1);
    termputs("\r\n");
    termputs(s2);
    termputs("\r\n");
    ozgetch();
}
