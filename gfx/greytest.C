#include <oz.h>
#include <stdlib.h>

main()
{
    static unsigned i,x,y,r,c;
    static char name[35];
    srand(ozsec()+60*ozmin()+60*60*ozhour()); /* seed random number generator */
    ozgreycls(); /* clear grey-scale display--this is the one grey
                    scale call that can be done before ozsetgreyscale() */
    if(ozsetgreyscale(1)) return; /* activate grey scale, return on error */
    if(!ozgetgreyscale()) return; /* get out if not active--should not happen */
    /* draw some solid circles */
    ozgreyfilledcircle(50,50,50,GREYSHADE_GREY1);
    ozgreyfilledcircle(50,50,40,GREYSHADE_WHITE);
    ozgreyfilledcircle(50,50,30,GREYSHADE_GREY2);
    ozgreyfilledcircle(50,50,20,GREYSHADE_BLACK);
    /* draw some random circles */
    for(i=0;i<100;i++)
    {
        x=rand()%198+20; /* choose a random number from 20 to 218 */
        y=rand()%40+20; /* choose a random number from 20 to 59 */
        r=rand()%19+1; /* choose a random number from 1 to 20 */
        c=rand()%4;    /* choose a random shade from 0 to 3 */
        ozgreycircle(x,y,r,c);
    }
    /* and two lines */
    ozgreyline(0,0,238,78,GREYSHADE_GREY1);
    ozgreyline(0,1,238,79,GREYSHADE_GREY2);
    /* and an unfilled box around everything */
    ozgreybox(0,0,239,80,GREYSHADE_GREY2);
    /* and a smaller filled box */
    ozgreyfilledbox(120,30,15,10,GREYSHADE_GREY2);
    /* and a bunch of random points */
    for(i=0;i<1000;i++)
    {
        x=rand()%238;
        y=rand()%80;
        c=rand()%4;
        ozgreypoint(x,y,c);
        /* this is to test ozgreygetpoint() */
        if(c!=ozgreygetpoint(x,y))
        {
            ozgreyputs(0,0,GREYSHADE_BLACK,"Error reading back point!");
            ozgetch();
            return;
        }
    }
    /* prompt for name entry */
    x=ozgreyputs(0,0,GREYSHADE_GREY2,"Enter your name");
    /* with question mark in lighter shade */
    x=5+ozgreyputch(x,0,GREYSHADE_GREY1,'?');
    /* reset the name */
    name[0]=0;
    /* load in the name */
    ozgreyeditline(x,0,name,35,239-x,GREYSHADE_GREY1);
    /* get height of current font */
    y=ozgetfontheight(ozgetfont());
    /* print "Hello, <name>", with <name> darker than the rest */
    x=ozgreyputs(0,y,GREYSHADE_GREY2,"Hello, ");
    ozgreyputs(x,y,GREYSHADE_BLACK,name);

    /* test the power key disabling */
    ozsetpowerkeyhandling(0);
    if(!ozgetpowerkeyhandling())
        ozgreyputs(x,2*y,GREYSHADE_BLACK,"POWER KEY DISABLED");
    ozgetch();
    ozsetpowerkeyhandling(1);
    if(ozgetpowerkeyhandling())
        ozgreyputs(x,2*y,GREYSHADE_BLACK,"POWER KEY RE-ENABLED");
    ozgetch();
    ozgreycls();
    ozgreyputs(x,3*y,GREYSHADE_BLACK,"Good-bye!  Press a key!");
    ozgetch();
    return;
}










