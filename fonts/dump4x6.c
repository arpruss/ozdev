#include <stdio.h>

unsigned char font[256*6]={ 0,0,0,0,0,0,
0,5,0,5,2,0,
0,5,0,0,7,0,
0,5,7,7,2,0,
0,2,7,7,2,0,
6,15,15,6,15,0,
2,7,2,2,7,0,
0,0,0,0,0,0,
15,15,9,9,15,15,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
14,10,10,10,5,0,
9,6,9,9,6,9,
1,3,7,3,1,0,
4,6,7,6,4,0,
2,7,2,2,7,2,
5,5,5,0,5,0,
14,11,11,10,10,0,
14,1,6,6,8,7,
0,0,0,15,15,0,
2,7,2,7,2,15,
2,7,2,2,2,2,
2,2,2,2,7,2,
0,4,15,4,0,0,
0,2,15,2,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
0,0,0,0,0,0,
1,1,1,0,1,0,
5,5,0,0,0,0,
6,15,6,15,6,0,
6,3,7,6,3,0,
1,4,2,1,4,0,
2,5,2,5,10,0,
2,2,1,0,0,0,
4,2,2,2,4,0,
1,2,2,2,1,0,
5,2,7,2,5,0,
0,2,7,2,0,0,
0,0,0,2,1,0,
0,0,7,0,0,0,
0,0,0,0,1,0,
4,2,2,2,1,0,

2,5+2,5,5,2,0,

2,3,2,2,2,0,
3,4,2,1,7,0,
3,4,3,4,3,0,
4,6,5,7,4,0,
7,1,3,4,3,0,
6,1,3,5,2,0,
7,4,2,2,2,0,
2,5,2,5,2,0,
2,5,6,4,2,0,
0,2,0,2,0,0,
0,2,0,2,1,0,
4,2,1,2,4,0,
0,7,0,7,0,0,
1,2,4,2,1,0,
3,4,2,0,2,0,
6,9,13,13,1,6,
2,5,7,5,5,0,
3,5,3,5,3,0,
6,1,1,1,6,0,
3,5,5,5,3,0,
7,1,3,1,7,0,
7,1,3,1,1,0,
6,1,5,5,6,0,
5,5,7,5,5,0,
7,2,2,2,7,0,
7,2,2,2,1,0,
5,5,3,5,5,0,
1,1,1,1,7,0,
5,7,7,5,5,0,
3,5,5,5,5,0,
2,5,5,5,2,0,
3,5,3,1,1,0,
2,5,5,3,6,0,
3,5,3,5,5,0,
6,1,2,4,3,0,
7,2,2,2,2,0,
5,5,5,5,6,0,
5,5,5,5,2,0,
5,5,7,7,5,0,
5,5,2,5,5,0,
5,5,2,2,2,0,
7,4,2,1,7,0,
6,2,2,2,6,0,
1,2,2,2,4,0,
3,2,2,2,3,0,
2,5,0,0,0,0,
0,0,0,0,15,0,
2,2,4,0,0,0,
0,3,6,5,6,0,
1,3,5,5,3,0,
0,6,1,1,6,0,
4,6,5,5,6,0,
0,2,5,3,6,0,
4,2,7,2,2,0,
0,6,5,6,3,0,
1,3,5,5,5,0,
2,0,2,2,2,0,
4,0,4,4,2,0,
1,1,5,3,5,0,
2,2,2,2,4,0,
0,3,7,7,5,0,
0,3,5,5,5,0,
0,2,5,5,2,0,
0,3,5,3,1,0,
0,6,5,6,12,0,
0,3,5,1,1,0,
0,6,3,6,3,0,
2,7,2,2,4,0,
0,5,5,5,6,0,
0,5,5,5,2,0,
0,5,7,7,2,0,
0,5,2,2,5,0,
0,5,5,6,3,0,
0,7,4,2,7,0,
4,2,3,2,4,0,
2,2,0,2,2,0,
1,2,6,2,1,0,
10,5,0,0,0,0,
0,2,5,7,0,0,
6,1,6,4,2,0,
5,0,5,5,6,0,
6,0,2,7,6,0,
2,5,2,5,6,0,
5,0,2,5,6,0,
3,0,2,5,6,0,
2,0,2,5,6,0,
0,6,1,6,2,0,
2,5,2,7,6,0,
5,0,2,7,6,0,
3,0,2,7,6,0,
5,0,2,2,2,0,
2,5,2,2,2,0,
3,0,2,2,2,0,
5,2,5,7,5,0,
2,2,5,7,5,0,
6,0,7,3,7,0,
0,14,13,5,14,0,
14,5,15,5,13,0,
7,0,2,5,2,0,
5,0,2,5,2,0,
3,0,2,5,2,0,
2,5,0,5,6,0,
3,0,5,5,6,0,
5,0,5,6,3,0,
9,6,9,9,6,0,
5,0,5,5,6,0,
2,7,1,7,2,0,
6,2,7,2,7,0,
5,5,2,7,2,0,
3,5,7,13,5,0,
6,2,7,2,3,0,
6,0,2,5,6,0,
6,0,2,2,2,0,
6,0,2,5,2,0,
6,0,5,5,6,0,
7,0,3,5,5,0,
7,0,7,5,5,0,
2,5,6,0,7,0,
2,5,2,0,7,0,
2,0,2,1,6,0,
0,0,7,1,0,0,
0,0,7,4,0,0,
5,5,2,7,13,0,
5,5,6,15,5,0,
1,0,1,1,1,0,
0,6,3,6,0,0,
0,3,6,3,0,0,
5,0,5,0,5,0,
5,10,5,10,5,10,
15,10,15,10,15,10,
2,2,2,2,2,2,
2,2,3,2,2,2,
2,2,3,3,2,2,
6,6,7,6,6,6,
0,0,7,6,6,6,
0,0,3,3,2,2,
6,6,7,7,6,6,
6,6,6,6,6,6,
0,0,7,7,6,6,
6,6,7,7,0,0,
6,6,7,0,0,0,
2,2,3,3,0,0,
0,0,3,2,2,2,
2,2,14,0,0,0,
2,2,15,0,0,0,
0,0,15,2,2,2,
2,2,14,2,2,2,
0,0,15,0,0,0,
2,2,15,2,2,2,
2,2,14,14,2,2,
6,6,14,6,6,6,
6,6,14,14,0,0,
0,0,14,14,6,6,
6,6,15,15,0,0,
0,0,15,15,6,6,
6,6,14,14,6,6,
0,0,15,15,0,0,
6,6,15,15,6,6,
2,2,15,15,0,0,
6,6,15,0,0,0,
0,0,15,15,2,2,
0,0,15,6,6,6,
6,6,15,0,0,0,
2,2,14,14,0,0,
0,0,14,14,2,2,
0,0,14,6,6,6,
6,6,15,6,6,6,
2,2,15,15,2,2,
2,2,3,0,0,0,
0,0,14,2,2,2,
15,15,15,15,15,15,
0,0,0,15,15,15,
3,3,3,3,3,3,
12,12,12,12,12,12,
15,15,15,0,0,0,
0,10,5,5,10,0,
2,5,3,5,3,1,
7,5,1,1,1,0,
0,15,5,5,5,0,
7,1,2,1,7,0,
0,14,5,5,2,0,
0,5,5,3,1,0,
0,13,2,2,2,0,
7,2,5,2,7,0,
2,5,7,5,2,0,
2,5,5,2,5,0,
6,1,2,5,2,0,
0,6,15,6,0,0,
8,6,15,6,1,0,
6,1,7,1,6,0,
2,5,5,5,5,0,
7,0,7,0,7,0,
2,7,2,0,7,0,
3,6,3,0,7,0,
6,3,6,0,7,0,
6,2,2,2,2,2,
2,2,2,2,3,0,
2,0,7,0,2,0,
10,5,0,10,5,0,
2,5,2,0,0,0,
0,0,6,6,0,0,
0,0,2,0,0,0,
14,2,2,3,2,0,
0,3,5,5,5,0,
3,4,2,7,0,0,
0,7,7,7,0,0,
0,0,0,0,0,0 };

#include "ascii2me.c"

void dumpline(int width, unsigned char value)
{
    unsigned char mask;
    for(mask=1;width;mask<<=1,width--)
    {
        if(mask&value) putchar('X');
            else putchar('.');
    }
    putchar('\n');
}



main()
{
    int i;
    int j;
    printf("0\n255\n6\n6\n");
    for(i=0;i<256;i++)
    {
        if(i<32 && i!=13) printf("%d %d (redirected)\n",i,ascii2me[i]);
        else if(i==127) printf("127 %d (redirected)\n",'e');
        else
        {
            printf("%d %d\n",i,i);
            if(i==13)
            {
                dumpline(4,0);
                dumpline(4,8);
                dumpline(4,8);
                dumpline(4,10);
                dumpline(4,15);
                dumpline(4,2);
            }
            else for(j=0;j<6;j++) dumpline(4,font[6*i+j]);
        }
    }
    return 0;
}

