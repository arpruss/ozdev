#include <string.h>
#include <stdlib.h>
#include <oz.h>
#include "same.h"

void randomboard(void)
{
	static byte i,j;
	for(i=0;i<width+2;i++)
	  for(j=0;j<height+2;j++)
		board[i][j]=0;
	for(i=1;i<=width;i++)
	  for(j=1;j<=height;j++)
		board[i][j]=(rand()%numpieces)+1;
	cursor_col=width/2;
	cursor_row=height/2;
}

int move(void)
{
	static byte refresh;
	static unsigned k;
	static byte c;
	static unsigned selected;
	refresh=1;
	selected=0;
	while(1)
	{
		showcount(selected);
		if(refresh)
			showboard();
		refresh=0;
		k=cursor_and_key(cursor_col,cursor_row);
		switch(k)
		{
            case 'i':
                ozsnap();
                ozsnap();
                break;
			case KEY_UP:
				if(cursor_row>0)
					cursor_row--;
				break;
			case KEY_DOWN:
				if(cursor_row<height-1)
					cursor_row++;
				break;
			case KEY_LEFT:
				if(cursor_col>0)
					cursor_col--;
				break;
			case KEY_RIGHT:
				if(cursor_col<width-1)
					cursor_col++;
				break;
			case KEY_PAGEUP:
				c=ozgetcontrast();
				if(c<MAX_CONTRAST) ozsetcontrast(c+1);
				break;
			case KEY_BACKLIGHT:
				oztogglelight();
				break;
			case KEY_PAGEDOWN:
				c=ozgetcontrast();
				if(c>0) ozsetcontrast(c-1);
				break;
			case ' ':
				c=board[cursor_col+1][cursor_row+1];
				if(!c) break;
				clearshade();
				selected=shade(cursor_col,cursor_row);
				showboard();
				showcount(selected);
				if(selected) return selected;
				  else beep();
				refresh=1;
				break;
			case 'c':
				clearshade();
				selected=0;
				refresh=1;
				break;
			case KEY_LOWER_MENU:
			case KEY_UPPER_MENU:
				help();
				break;
			case KEY_LOWER_ESC:
			case KEY_UPPER_ESC:
			case 27:
			case 'q':
				return -1;
			case KEY_MYPROGRAMS:
			case KEY_MEMO:
			case KEY_MAIN:
			case KEY_SCHEDULE:
			case KEY_TELEPHONE:
				ozexitto(k);
			case KEY_LOWER_ENTER:
				c=board[cursor_col+1][cursor_row+1];
				if(!c)
				{
                    if(selected)
                    {
                        clearshade();
                        selected=0;
                        refresh=1;
                    }
					beep();
					break;
				}
				if(c&GREY_MASK)
					return selected;
				clearshade();
				selected=shade(cursor_col,cursor_row);
				showcount(selected);
				if(selected==0) beep();
				refresh=1;
		}
	}
}

long play(void)
{
	static int selected;
	static long delta;
	static byte start_width;
	static char msg[40];
	score=0;
	start_width=width;
	ozcls();
    showscore();
    rest=height*width;
    showrest();
	showhigh();
	do
	{
		clearshade();
		if(!check_for_moves())
		{
			beep();
			selected=0;
			break;
		}
#if 0
        if(wizplay)
        {
            static byte i,j;
            showboard();
            findmove(&i,&j);
            selected=quickshade(i,j);
        }
        else
#endif
            selected=move();
		if(selected>0)
		{
			score+=(unsigned)((selected-2))*(selected-2);
			compress();
            showscore();
            showrest();
		}
	} while(width && selected>0);
    if(width==0) delta=5L*start_width*height;
	else
        delta=-(int)rest;
	score+=delta;
    showscore();
    if(selected>=0) bounce();
	ozcls();
	ozsetfont(FONT_PC_LARGE);
	sprintf(msg,"Raw score: %ld",score-delta);
	ozputs(0,0,msg);
	sprintf(msg,"Adjustment: %ld",delta);
	ozputs(0,13,msg);
	sprintf(msg,"Total score: %ld",score);
	ozputs(0,26,msg);
    ozsetfont(FONT_PC_NORMAL);
	showhigh();
	return score;
}

main()
{
	static char name[MAXNAMELEN+1]="";
	static long s;
	static byte x;
	load_stuff();
	atexit(save_stuff);
	srand(oztime());
	while(menu())
	{
		y0=80-PIECE_SIZE*height;
		randomboard();
		s=play();
        if(s>(long)(highscores[current_size][current_number]))
		{
			highscores[current_size][current_number]=s;
			changed=1;
			highscore_names[current_size][current_number][0]=0;
            ozsetfont(FONT_PC_LARGE);
            ozputs(0,39,"You beat the high score!");
            x=ozputs(0,52,"Enter your name: ");
            if(ozeditline(x,52,name,MAXNAMELEN+1,238-x)>0)
				strcpy(highscore_names[current_size][current_number],name);
			else strcpy(highscore_names[current_size][current_number],
					"Anon");
		}
		else
		{
            ozsetfont(FONT_PC_LARGE);
            ozputs(0,39,"Press any key to continue");
			ozgetch();
		}
        ozsetfont(FONT_PC_NORMAL);
	}
}
