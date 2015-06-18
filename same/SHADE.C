#include <oz.h>
#include "same.h"

#define noshow 0

void clearshade(void)
{
	static byte i,j;
	for(i=0;i<width+2;i++)
	  for(j=0;j<height+2;j++) board[i][j]&=~GREY_MASK;
    if(!noshow) hide_selection();
}

static void click(void)
{
	int i;
	if(current_slow)
	{
		if(!current_quiet) ozkeyclick();
		for(i=0;i<3000 && !ozkeyhit();i++);
	}
}

static void squash_down(byte i,byte j)
{
	static byte k;
    static byte c;
	for(k=j;k>1;k--)
    {
        c=board[i][k]=board[i][k-1];
        if(!noshow) putpiece(i-1,k-1,c);
    }
    board[i][k]=0;
    if(!noshow) putpiece(i-1,k-1,0);
}

static void squash_left(byte i)
{
    static byte i1,j,c;
    if(i!=width && !noshow) click();
	for(i1=i;i1<width;i1++)
		for(j=1;j<=height;j++)
        {
            c=board[i1][j]=board[i1+1][j];
            if(!noshow) putpiece(i1-1,j-1,c);
        }
	for(j=1;j<=height;j++)
    {
        board[i1][j]=0;
        if(!noshow) putpiece(i1-1,j-1,0);
    }
	width--;
}

void compress(void)
{
	static byte i,j,count,toclick;
    if(!noshow) hide_selection();
	for(i=width;i;i--)
		for(j=height;j;j--)
			if(board[i][j]&GREY_MASK)
			{
                if(!noshow) putpiece(i-1,j-1,0);
				board[i][j]=0;
			}
	for(i=width;i;i--)
	{
		for(j=height;j && board[i][j];j--);
		for(;j && !board[i][j];j--);
		if(j)
		{
			toclick=0;
			count=0;
			for(j=1;j<=height;j++)
			{
				if(board[i][j])
					count=1;
				else if(count)
				{
					toclick=1;
					squash_down(i,j);
				}
			}
            if(toclick && !noshow) click();
		}
	}
	rest=0;
	for(i=width;i && width;i--)
	{
		count=0;
		for(j=1;j<=height;j++)
			if(board[i][j])
			{
				rest++;
				count=1;
			}
		if(!count) squash_left(i);
	}
    if(!noshow && current_slow && ozkeyhit()) ozgetch();
}

#if 0
unsigned quickshade(byte _i0,byte _j0)
{
	static unsigned selected;
	static byte blocknumber,i,j;
	blocknumber=blocked_board[_i0][_j0];
	selected=0;
    for(i=blocklocations[blocknumber][0];i<width;i++) for(j=0;j<width;j++)
	{
		if(blocked_board[i][j]==blocknumber)
		{
            board[i+1][j+1]|=GREY_MASK;
			selected++;
		}
		else
            board[i+1][j+1]&=~GREY_MASK;
	}
	if(selected<2)
	{
        board[_i0+1][_j0+1]&=~GREY_MASK;
		return 0;
	}
	else
		return selected;
}
#endif


unsigned shade(byte _i0,byte _j0)
{
	static byte i,j,c;
	static byte changed;
	static byte i0,j0,i_start,i_end,j_start,j_end,count;
	static unsigned selected;
	clearshade();
	count=1;
	selected=0;
	i0=_i0;
	j0=_j0;
	c=board[i0+1][j0+1]&~GREY_MASK;
	board[i0+1][j0+1]=c|GREY_MASK;
	do
	{
		if(i0>count) i_start=i0-count; else i_start=1;
		if(i0+count<width) i_end=i0+count; else i_end=width;
		if(j0>count) j_start=j0-count; else j_start=1;
		if(j0+count<height) j_end=j0+count; else j_end=height;
		changed=0;
		for(i=1;i<=width;i++) for(j=1;j<=height;j++)
		  {
			if(board[i][j]!=c) continue;
			if((board[i-1][j]
			  | board[i+1][j]
			  | board[i][j-1]
			  | board[i][j+1]) &GREY_MASK)
			{
			   selected++;
			   changed=1;
			   board[i][j]|=GREY_MASK;
			}
		  }
		count++;
	} while(changed);
	if(!selected)
	{
		board[i0+1][j0+1]=c;
		return 0;
	}
	return selected+1;
}

byte check_for_moves(void)
{
	static byte i,j;
	for(i=1;i<=width;i++) for(j=1;j<=height;j++)
	{
		if(!board[i][j]) continue;
		if(board[i][j]==board[i][j+1]
		|| board[i][j]==board[i+1][j]) return 1;
	}
	return 0;
}
