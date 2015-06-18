/* VERY DANGEROUS!!!!!!!!!!!! */

close(){}

main()
{
    char buf[16];
    reset_term();
/*    sprintf(buf,"%x",ozflashwrite(0x89,0xacb0,0)); */
    termputs(buf);
    ozgetch();
}
