/*
3d.c

Demo using standard Wizard 3d and 4d math functions

Copyright© 2002, Mark Hamilton

*/
#include <oz.h>
#include "lib3d.h"

#define TIMING

int main(void);


#define MX	238/2
#define MY	80/2

Vector_t cube[8]
= { { -20 ,  20,   20 },
	{  20 ,  20,   20 },
	{  20 , -20,   20 },
	{ -20 , -20,   20 },
	{ -20 ,  20,  -20 },
	{  20 ,  20,  -20 },
	{  20 , -20,  -20 },
	{ -20 , -20,  -20 } };

int main(void)
{
	static Vector_t rot;
	static Vector_t t;
	static Point_t p[8];
	static unsigned c = 0;
	static int i;
	static int zf = 0;
#ifdef TIMING
    extern unsigned _oz64hz_word;
    static unsigned frames;
#endif
    ozinitprofiler();
	ozsetactivepage(1);
	ozsetdisplaypage(0);
#ifdef TIMING
    frames=0;
    _oz64hz_word=0;
#endif
	while(c != KEY_POWER) {
		if(ozkeyhit()) c = ozngetch();
		switch(c) {
			case KEY_UP:
				zf -= 10;
				if(zf < -100) zf = -100;
				break;
			case KEY_DOWN:
				zf += 10;
				if(zf > 300) zf = 300;
				break;
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
				exit(0);
				break;
		}
		c = 0;
		for(i = 0; i < 8; i++) {
			ozcopyvector(&t,&cube[i]);
			ozrotatepointx(&t, rot.x);
			ozrotatepointy(&t, rot.y);
			t.z += zf; /* zoom factor */
			ozplotpoint(&t, &p[i]);
		}
		rot.y = (rot.y+1)%360;
		rot.x = (rot.x+2)%360;
		ozcls();
		/* top face */
		ozline(p[0].x + MX, p[0].y + MY, p[1].x + MX, p[1].y + MY, BLACK);
		ozline(p[1].x + MX, p[1].y + MY, p[2].x + MX, p[2].y + MY, BLACK);
		ozline(p[2].x + MX, p[2].y + MY, p[3].x + MX, p[3].y + MY, BLACK);
		ozline(p[3].x + MX, p[3].y + MY, p[0].x + MX, p[0].y + MY, BLACK);
		/* bottom face */
		ozline(p[4].x + MX, p[4].y + MY, p[5].x + MX, p[5].y + MY, BLACK);
		ozline(p[5].x + MX, p[5].y + MY, p[6].x + MX, p[6].y + MY, BLACK);
		ozline(p[6].x + MX, p[6].y + MY, p[7].x + MX, p[7].y + MY, BLACK);
		ozline(p[7].x + MX, p[7].y + MY, p[4].x + MX, p[4].y + MY, BLACK);
		/* side faces */
		ozline(p[0].x + MX, p[0].y + MY, p[4].x + MX, p[4].y + MY, BLACK);
		ozline(p[1].x + MX, p[1].y + MY, p[5].x + MX, p[5].y + MY, BLACK);
		ozline(p[2].x + MX, p[2].y + MY, p[6].x + MX, p[6].y + MY, BLACK);
		ozline(p[3].x + MX, p[3].y + MY, p[7].x + MX, p[7].y + MY, BLACK);
        ozsetdisplaypage(!ozgetdisplaypage());
#ifdef TIMING
        frames++;
        if(_oz64hz_word>64*10)
        {
            static char buf[80];
            sprintf(buf,"%d frames in 10 seconds",frames);
            ozputs(0,0,buf);
            ozgetch();
            _oz64hz_word=0;
            frames=0;
        }
#endif
		ozsetactivepage(!ozgetactivepage());
	}
}
