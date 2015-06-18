#include <stdlib.h>
#include <oz.h>




void (main) (void) {
	static int x , x1 , x2 , y , y1 , y2 , color;
	ozcls();
	ozputs(0,0,"Press any key to start, then any key to exit");
	getch();
	ozdelay(20);  /*allow a bit of time to release key*/
	while (!ozkeyhit()) {
		y1 = ((rand()/273) - 20);
		y2 = ((rand()/273) - 20);
		x1 = ((rand()/117) - 20);
		x2 = ((rand()/117) - 20);
		if (y2 < y1)
		{
			y = y1;
			y1 = y2;
			y2 = y;
		}
		if (x2 < x1)
		{
			x = x1;
			x1 = x2;
			x2 = x;
		}
		color = rand() / 10922;
		ozfilledbox(x1 , y1 , x2 - x1 , y2 - y1 , color);
	}
}
