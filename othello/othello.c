/* #define PROFILER /* */

/* modified for Wizard 7xx by Alexander R. Pruss */

/* From: bet@std.sbi.com (Bennett Todd)
Newsgroups: alt.sources
Subject: Othello.c
Date: 22 Jun 1994 03:23:45 GMT
Organization: Salomon Brothers, Inc.

This is a fairly portable program I wrote to play othello for an AI class
back in 1983. I wrote it using DeSmet C, and it has some special PC
optimizations, under #ifdef DeSmet, that are non-portable. You'll need to
blow out the #define DeSmet if you aren't using his C compiler. If your C
compiler gets annoyed at illegal syntax within #if/#endif that isn't being
compiled, then you'll also need to edit out the blocks that are ifdef
DeSmet.  (All DeSmet blocks have been removed from this version.)

This program uses Alpha-Beta pruning of a minimax lookahead tree. It has a
fairly tweaked board evaluation heuristic, with an edge strategy and so on.
It is possibly over-optimized by modern standards, but this thing plays a
strong game, on a 4.77MHz 8088 in 64K of RAM, with no move taking more than
30 seconds.

-Bennett
bet@sbi.com */

/*
 * Othello playing program. Fairly portable if DeSmet isn't defined;
 * defining DeSmet enables some inline-assembler screen wizzery on PCs,
 * using DeSmet C's convention for inline assembler.
 *
 * The code looks a _lot_ better if you display it with tabstops every
 * four spaces, rather than every 8.
 *
 * Written in 1993 by Bennett Todd, with some help by John White, who
 * designed the finite state automaton for edge strategy.
 */

/*

	Plays othello using alpha-beta search.
	Defining DEBUG_LEVEL as 1 sets enhanced error trapping, and
							2 gives run-time statistics
							3 enables enhanced run-time sanity checking

	Switches:
	-f	gofirst (default=no)
	-bn	allocate n boards: default=64, grows as branching factor*depth
	-dn	look ahead n moves: default is variable, must be greater than 0
	-rn reverse the initial board in various ways:

		-r1 reverse initial board setup; 'o' still goes first
		-r2 reverse initial board setup; 'x' goes first
		-r3  normal initial board setup; 'x' goes first

*/

#include <ctype.h>
#include <oz.h>
#include "othello.h"
#define NULL ((void*)0)

#define NOTIME
#define fast_puts _printf

#ifdef STACK_TRACK
void *lowest_stack=(void *)0xffff;
#endif


#define time_tick() 0

#define DEBUG_LEVEL 0

#if DEBUG_LEVEL > 0
#define MAXBOARDS 64
#else
#define MAXBOARDS 74
#define NONDYNAMIC
BOARD _space_freeboards[MAXBOARDS];
boardchar _space_boardspace[64*MAXBOARDS];
boardchar *_space_pointers[484];
boardchar _space_bytes[2016];
#endif


void setupsound(void)
{
    ozportout(0x19,1);
    ozportout(0x17,10);
    ozportout(0x18,0);
    ozportout(0x16,2);
}


int handlekey(unsigned c)
{
    switch(c)
    {
            case KEY_MYPROGRAMS:
            case KEY_UPPER_ESC:
            case KEY_LOWER_ESC:
              exit(0);
            case KEY_MAIN:
            case KEY_SCHEDULE:
            case KEY_MEMO:
            case KEY_TELEPHONE:
              ozexitto(c);
            case KEY_BACKLIGHT:
              oztogglelight();
              return 0;
            case 'i':
              ozsnap();
              ozsnap();
              return 0;
            default:
              return 1;
    }
}

void clearkey(void)
{
    while(ozkeyhit()) handlekey(ozgetch());
}


unsigned mygetch(void)
{
    static unsigned c;
    do
    {
        c=ozgetch();
    } while(!handlekey(c));
    return c;
}

void putbox(byte row, byte col)
{
    static byte x1, y1, i;
    x1=col*8-1;
    y1=row*8-1;
    for(i=0; i<8; i++)
    {
        ozpoint(x1+i,y1,BLACK);
        ozpoint(x1,y1+i,BLACK);
        ozpoint(x1+i,y1+8,BLACK);
        ozpoint(x1+8,y1+i,BLACK);
    }
    ozpoint(x1,y1,BLACK);
    ozpoint(x1,y1+8,BLACK);
    ozpoint(x1+8,y1,BLACK);
    ozpoint(x1+8,y1+8,BLACK);
}

char cursor_shape[][2]
= { { 0,0 },
    { 0,1 },
    { 1,0 },
    { 6,0 },
    { 6,1 },
    { 5,0 },
    { 0,6 },
    { 1,6 },
    { 0,5 },
    { 6,6 },
    { 6,5 },
    { 5,6 } };
#define NUMPOINTS_CURSOR (sizeof(cursor_shape)/2)

char xoffset;
#define YOFFSET 2

void xorothellocursor(byte row, byte col)
{
    static byte x1, y1, i;
    x1=col*8;
    y1=row*8;
    for(i=0;i<NUMPOINTS_CURSOR;i++)
      ozpoint(x1+cursor_shape[i][0],y1+cursor_shape[i][1],XOR);
}

/*
	About the following defines:
	BEST and WORST are upper and lower bounds, respectively, on the
		worth of a board.
	MAXSONS is the maximum number of possible moves from any given board,
		used to size the sons array in a board node.
	EMPTY, MINE, and HIS are values for the possible states of a square
		on an othello board. In the original code, they were used as an
		enumerated type, after the fashion of PASCAL. However, during
		the process of optimizing the program, It was observed that
		in the code
			<expression>!=EMPTY
		!=EMPTY doesn't change the logical value of the expression, and
			<expression>==EMPTY is equivalent to !<expression>
		Thus the contents of board cells are sometimes used as boolean
		variables.
		What's worse, many nested if expressions and switch statements
		were removed by using values of board cells as subscripts into
		arrays, a process which altogether exceeded any reasonable bounds
		in the finite state machine found in the function worth.
*/

BOARD *freeboards; /* pointer to head of linked list of free boards */

char maxdepth[60]=   /* maxdepth[movenumber] is the depth that alphabeta */
	{				/* will search on the movenumber'th move			*/
        /* second entry was 4, changed to 1 by ARP as it made no
        difference in the actual moves */
        1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 4, 7, 6, 5, 4, 3, 2, 1, 1
    };

char person_last_x=-1;
char person_last_y=-1;
char pc_last_x=-1;
char pc_last_y=-1;

int
    maxboards=MAXBOARDS,   /* default number of boards to allocate [64]    */
	reversed=FALSE, /* switch to reverse initial board position */
	movenumber=0,	/* counter for what move we are on			*/
#ifndef NOTIME
	time_out[3]=	/* timeout points, to hurry up if we are in	*/
	{				/* danger of overshooting 30 seconds		*/
		2000,2500,2800
	},
#endif
	start_dive=52,	/* the movenumber on which to switch worths */
	gofirst=FALSE,	/* by default, the opponent goes first		*/
	max,			/* who is playing at any given instant		*/
    (*worth)();     /* pointer to the current worth function    */
char
	i_tran[64]=		/* i subscript generator					*/
	{
		0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7
	},
	j_tran[64]=		/* j subscript generator					*/
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7,
		0, 1, 2, 3, 4, 5, 6, 7
	};

#ifndef NOTIME
unsigned time;          /* time tick holder, for stopwatch  */
#endif

/*
	The array moves, declared below, is an array of pointers to arrays
	of pointers to arrays of subscripts, ranging from 0 to 63.
	A board is declared in the structure declaration above to be represented
	by an array of 8 pointers to arrays of chars (8 per array). In my
	initialization of the board structures, I explicitly allocate the row
	arrays consecutively; therefore, board->array[0] can be taken as a
	pointer to an array 64 long of chars, containing the cells of the board.
    board->array[0][moves[i][j][k]] is the k-th step in th j-th direction
	from the i-th cell in board. Moves is 64 long; moves[i] is variable
	length (one for each direction in which a move is possible) delimited
	with NULLs, and move[i][j] is variable length, delimited by i. In
	other words, to walk as far as possible in the j-th direction from
	cell i, you could use
		for(k=0;i!=moves[i][j][k];k++)
	though I didn't, since I use pointers to walk through the array.

*/

boardchar **moves[64],
     print_chars[3]={ 29,30,31 }; /* 250,254,'o'*/ /* the character to print for a cell */

#if DEBUG_LEVEL > 1
int allsons_called,
	worth_called,
	max_used_boards,
	boards_in_use,		/* various variables to accumulate statistics */
	greatest_branching,
    alpha_prunes,
	beta_prunes;
#endif
#if DEBUG_LEVEL > 2
	int moves_checksum,
		board_count=0;

	BOARD *board_bottom,
		  *board_end;
#endif

main()
{
    static int i,
        /* temp, */
		oldmove;
    static unsigned c,x;

    static BOARD *root;            /* the root of the current tree, changed by     */
    BOARD *firstboard();    /* both move() and getmove()                    */

#ifdef PROFILER
ozinitprofiler();
#endif

    setupsound();
    reset_term();
    xorcursor();
    ozfont(FONT_PC_LARGE);
    ozputs(0,0,"Othello 1.71");
    ozfont(FONT_PC_NORMAL);
    ozputs(0,12,"Game by Bennett Todd with some help from John White.");
    ozputs(0,22,"Adapted for the Wizard by Alexander R. Pruss.");
    ozputs(0,32,"USE AT YOUR OWN RISK.");
    ozputs(0,42,"Press MY PROGRAMS to exit.");
    x=ozputs(0,60,"(T)rivial, (E)asy or (N)ormal? [N] ");
    while((c=mygetch())!='e' && c!='n' && c!='t' && c!='\r'
     && c!=KEY_LOWER_ENTER && c!=KEY_UPPER_ENTER);
    if(c=='e')
    {
        ozputs(x,60,"Easy");
        for(i=2; i<60; i++)
          if(maxdepth[i]>2) maxdepth[i]--;
    }
    else if(c=='t')
    {
        ozputs(x,60,"Trivial");
        for(i=1;i<60;i++) maxdepth[i]=1;
    }
    else ozputs(x,60,"Normal");
    x=ozputs(0,70,"Would you like to go first? ([Y]/N) ");
    while((c=mygetch())!='y' && c!='n' && c!='\r'
     && c!=KEY_LOWER_ENTER && c!=KEY_UPPER_ENTER);
    if(c=='n')
    {
        gofirst=1;
        ozputs(x,70,"No");
    }
    else ozputs(x,70,"Yes");

	/* call various initialization routines			*/
	worth=worth_1;	/* the main heuristic function	*/

	initmoves();	/* initialize the array moves	*/

#if DEBUG_LEVEL > 2
	board_bottom=freeboards;
	moves_checksum=check_moves();
#endif

	initfree();			/* set up free boards list	*/

#if DEBUG_LEVEL > 2
    _printf("allocated %d boards\n",board_count);
#endif

	root=firstboard();	/* set up to play the game	*/

#if DEBUG_LEVEL > 2
	board_count--;
    _printf("which leaves %d to play with.\n",board_count);
#endif

#ifndef NOTIME
	time=time_tick();	/* and mark time			*/
#endif

	if(gofirst)
		move(&root);
	else
	{

    reset_term();
		putboard(root,NULL);	/* put only one board */
	}

	oldmove=(-1); /* dummy out endgame flag, it's only starting now! */

	/* main game loop */
	while(movenumber<60 && movenumber!=oldmove) /* game lasts 60 moves,		*/
	{											/* unless no one can move	*/

#if DEBUG_LEVEL > 2
		if(moves_checksum!=check_moves())
		{
            _printf("Moves checksum failed.\n");
            clearkey();
            mygetch();
            exit(0);
		}
		if(board_count!=count_boards())
            _printf("I seem to have mislain a board: %d/%d\n",
					board_count,count_boards());
#endif

		if(movenumber>start_dive)	/* change to h*, the "true" h function	*/
		{
#ifndef NOTIME
            time_out[0]=time_out[1];
#endif
            worth=worth_2;          /* only computable by looking to the end*/
		}
		oldmove=movenumber;			/* set to recognize if no one can move	*/
		getmove(&root);	/* get a move from the user	*/
		move(&root);	/* make a move				*/
	}
	score(root);	/* report the result */
}

/*
	move makes a move on the (called-by-reference) board. It first
	expands all possible moves with a call to allsons, with max set to
	true (moves is trying to maximize the evaluation function), then uses
	alphabeta to evaluate each possibility, picking the best.
	Move is different from alphabeta mostly in that
		a)	it always and only plays as max
		b)	it must keep track of the actual boards, and not just
			their values (alphabeta always reclaims as soon as possible)
		c)	it is in charge of initializing everything for the search
		d)	rather than returning a backed up value, it simply outputs the
			move (and some other junk) and resets the root to point to the
			new move.
*/

move(board)
BOARD **board;
{
    register BOARD *tempboard;

     static int i,
		n,
#ifndef NOTIME
		rush=0,			/* used to panic stop short of 30 seconds			*/
#endif
		temp,
        alpha, /*=WORST,*/    /* the root is max, and therefore has an alpha value*/
        best, /*=WORST-1,*/   /* must be worst than the worst possible value, so  */
						/* that a move will be picked even if they are all	*/
						/* the worst.										*/
		bestboard;		/* subscript of the best board seen so far			*/

     static boardchar *t1,           /* a couple of temporary pointers used to           */
		 *t2;			/* figure out exactly where I moved.				*/

	max=TRUE;	/* let us get this straight, once and for all... */
    alpha=WORST; /* ARP */
    best=WORST-1; /* ARP */

/*    reset_term(); */

	/* read the following as "if( <number of sons found> == 0)"	*/
	if(!(n=allsons(*board)))	/* "==0" is equivalent to "!"	*/
	{
        movecursor(0,0);
        _printf("I cannot move.\n");
        pc_last_x=pc_last_y=-1;
        putboard(*board,*board); /* put the board so that he can see the */
        return;                  /* result of his last move              */
	}

#if DEBUG_LEVEL > 1
	allsons_called=1;
	worth_called=n;
	boards_in_use=max_used_boards=1;	/* initialize variables to	*/
	greatest_branching=n;				/* accumulate statistics	*/
	alpha_prunes=0;
	beta_prunes=0;
#endif

	/* for every son */
	for(i=0;i<n;i++)
	{

		max=FALSE;	/* think as my opponent would think */

		/* if this is better than the best we have seen so far */
		if(best<(temp=alphabeta((*board)->sons[i],maxdepth[movenumber],
								alpha,BEST)))
		{
			best=alpha=temp;
			bestboard=i;
		}

		/* make sure we do not under any circumstances exceed 30 seconds */
#ifndef NOTIME
		if((time_tick()-time) > time_out[rush])
		{
			if(rush==2)
#if DEBUG_LEVEL > 1
			{
/*                line_number=0; */
				fast_puts("buggering out, on account of lack of time!!");
#endif
				goto outta_here;

#if DEBUG_LEVEL > 1
			}
            _printf("I'm hurrying...\n");
#endif

			rush++;
			maxdepth[movenumber]--;
		}
#endif
	}

#ifndef NOTIME
outta_here:
#endif
	t1=(*board)->array[0]-1;					/* set up pointers	*/
	t2=(*board)->sons[bestboard]->array[0]-1;	/* for comparison	*/
loop:
	while(*++t1==(*++t2));	/* find different squares			*/
	if(*t1)	/* then this square was flipped, not actually moved */
		goto loop;	/* so keep trying							*/

	/* temp gets the 0-63 subscript of the move */
	temp=t1-(*board)->array[0];

    reset_term();
#if DEBUG_LEVEL > 1
    _printf("I moved at %c %d\n",
            'a'+j_tran[temp],1+i_tran[temp]);
#else
        _printf("I moved at %c %d\n",
			'a'+j_tran[temp],1+i_tran[temp]);
#endif
    pc_last_x=j_tran[temp];
    pc_last_y=i_tran[temp];

	putboard((*board),(*board)->sons[bestboard]);

#if DEBUG_LEVEL > 1
#ifndef NOTIME
	if((time_tick()-time) > 3000)
        _printf("\7\7\7\7\7\7");
#endif
    _printf("movenumber: %d choices: %d depth: %d",
			movenumber,n,maxdepth[movenumber]);
#if DEBUG_LEVEL > 2
    _printf(" boards available: %d",count_boards()+n);
#endif
    _printf("\nallsons called: %d worth called: %d boards used: %d\n",
			allsons_called,worth_called,max_used_boards);
    _printf("  alpha prunes: %d  beta prunes: %d       total: %d",
			alpha_prunes,beta_prunes,alpha_prunes+beta_prunes);
    _printf(" max branching: %d\n",greatest_branching);
#endif

	/* now reclaim unneeded boards, reset the root, and count this move */
	for(i=0;i<n;i++)
		if(i!=bestboard)
			reclaim((*board)->sons[i]);
	tempboard=(*board)->sons[bestboard];
	reclaim(*board);
	*board=tempboard;
	movenumber++;
}

/*
	alphabeta takes four parameters: the board to evaluate,
	the depth to search, and the current alpha and beta values.
	It returns the worth of the board, obtained by backing up
	the values seen at the bottom of the search.
	It plays maximizing or minimizing, based on the global
	boolean variable max.
*/
	
int alphabeta(BOARD *current,int depth,int alpha,int beta)
{
    byte i,n;
    int temp,
		best,
		curmax; 	/* contains the current value of the global "max",	*/
					/* negated (to minimize the number of negations)	*/

    register BOARD *curson;  /* contains the son being examined currently        */

#ifdef STACK_TRACK
    if((void*)&curmax<lowest_stack) lowest_stack=&curmax;
#endif

	/* if no sons can be found */
	if(!(n=allsons(current)))
	{
		max=!max;						/* then try as the other guy	*/
		if(!(n=allsons(current)))		/* and if he can't move			*/
			return(worth_2(current));	/* return the final value		*/
	}

	best=max?WORST:BEST;	/* start off assuming the worst 	*/
	depth--;				/* keep track of depth				*/
	curmax=!max;			/* and max through recursive calls	*/

	/* for every son */
	for(i=0;i<n;i++)
	{
		max=curmax;
		curson=current->sons[i];

		/*
			This statement does it all. Recurse, propogating correct alpha
			and beta values (only one of which can change at any given node)
			unless of course the recursion has terminated, in which case we
			use the value of the node, as evaluated by worth when allsons
			created the node. Put the value thus obtained into temp, and
			compare it with the best value seen so far. The sense of
			comparison is reversed if curmax is true (bitwise xor is all
			right if both booleans are "pure"--0 or 1).
		*/
		if(curmax^best<(temp=depth?
								(curmax?
									alphabeta(curson,depth,alpha,best)
									:alphabeta(curson,depth,best,beta))
								:curson->val))
		{
			/* check for an alphabeta "prune" of the tree */
			if(curmax?(temp<=alpha):(temp>=beta))
			{

#if DEBUG_LEVEL > 1
				if(curmax)
					beta_prunes++;
				else				/* accumulate statistics */
					alpha_prunes++;
#endif

				while(i<n)						/* clean up			*/
					reclaim(current->sons[i++]);
				return(temp);					/* and pack it in	*/
			}
			else
				best=temp;	/* remember the best value seen so far	*/
		}
		reclaim(curson);
	}
	return(best);
}

/*
	Simple-minded free space management. Keep a bunch of boards on a linked
	list. When one is needed, take it off the top. When no longer needed,
	insert it at the top. Optionally, check for out-of-boards (The converse,
	attempting to check for freeing of space not taken from the freeboard
	list, seemed to be too difficult. Perhaps I could have kept around high-
	water and low-water mark pointers to the space of legal board pointers.),
	and accumulate statistics.
*/

BOARD *newboard()
{
    register BOARD *temp;

#if DEBUG_LEVEL > 0
#ifndef NONDYNAMIC
	if(!freeboards)
#if DEBUG_LEVEL > 1
	{
        _printf("Gotta grab more boards...\n");
        _printf("Current boards outstanding : %d\n",boards_in_use);
        _printf("           High-water mark : %d\n",max_used_boards);
#endif
		initfree();
#if DEBUG_LEVEL > 1
	}

	boards_in_use++;
	if(max_used_boards<boards_in_use)
		max_used_boards=boards_in_use;
#endif
#endif
#endif

	temp=freeboards;
	freeboards=freeboards->sons[0];
	temp->sons[0]=NULL;
	return(temp);
}

reclaim(board)
BOARD *board;
{
#if DEBUG_LEVEL > 1
	boards_in_use--;
#endif
#if DEBUG_LEVEL > 2
	if(board<board_bottom || board > board_end)
        _printf("warning: attempted to reclaim non-board\n");
	else
	{
#endif

	board->sons[0]=freeboards;
	freeboards=board;

#if DEBUG_LEVEL > 2
	}
#endif

}

/*
	Shell sort taken from K&R, sorts the n boards in the array into ascending
	or descending order based on the global boolean max, sorting on the
	values contained in the value member of each board structure. This sort
	focuses the alphabeta search significantly, increasing the pruning enough
	to cut depth 4 search time by approximately 65 percent.
*/

sort(boards,n)
BOARD **boards;
int n;
{
    static int i,
		j,
		gap,
		jpg; /* temporary storage hold j+gap */

    register BOARD *tempboard;

	for(gap=n/2;gap>0;gap/=2)
		for(i=gap;i<n;i++)
			for(j=i-gap;j>=0 &&
					(max?(boards[j]->val<boards[jpg=j+gap]->val):
					(boards[j]->val>boards[jpg=j+gap]->val));j-=gap)
			{
				tempboard=boards[j];
				boards[j]=boards[jpg];
				boards[jpg]=tempboard;
			}
}

/*
	Initialize a linked list of free boards. Each board has an array of
	8 pointers to rows of 8 bytes each. Allocate space for the rows, and
	initialize the pointer arrays to point to the rows. The rows in a board
	are explicitly allocated contiguously, so it is possible (and turns out
	to enhance efficiency at the expense of comprehensability) to treat
	a board as a single array of 64 bytes, pointed to by the pointer to
	the first row. Optionally, produce statistics on allocation of memory.
*/

initfree()
{
     int i,j;

#ifndef NONDYNAMIC
    void *calloc();
#endif
    register boardchar *boardspace;

#ifndef NONDYNAMIC
	freeboards=((BOARD *) calloc(maxboards,sizeof(BOARD)));
	if(!freeboards)
	{
        _printf("The board structure allocation failed.\n");
        clearkey();
        mygetch();
		exit(0);
	}
	boardspace=calloc(maxboards,sizeof(char [64]));
	if(!boardspace)
	{
        _printf("The board freespace allocation failed.\n");
        clearkey();
        mygetch();
		exit(0);
	}
#else
    freeboards=_space_freeboards;
    boardspace=_space_boardspace;
#endif

#if DEBUG_LEVEL > 1
    _printf("    board  space %d\n",((unsigned int) freeboards));
    _printf("            thru %d\n",
			((unsigned int) (freeboards+maxboards)));
    _printf("    board arrays %d\n",((unsigned int) boardspace));
    _printf("            thru %d\n",
			((unsigned int) (boardspace+64*maxboards)));
#endif

	/* thread linked list and arrange the row pointer arrays */
	for(i=0;i<maxboards;i++)
	{
		freeboards[i-1].sons[0]=freeboards+i;
		for(j=0;j<8;j++)
			freeboards[i].array[j]=boardspace+64*i+8*j;
	}
	freeboards[maxboards-1].sons[0]=NULL;

#if DEBUG_LEVEL > 2
	board_end=freeboards+(maxboards-1);
	board_count+=maxboards;
#endif

	/* We might be called again, if he needs more boards */
	maxboards=10;
}

/*
	The array moves contains information about the shape of a board, in
	a fashion which simplifies the checking of loop conditions in the
	code which is examining a board for a move, and flipping the resulting
	pieces. Specifically, moves (which is a byte array because it needs
	no large numbers) is used as follows:
		moves[i][j][k] refers to the subscript of the
			k-th square, in the
			j-th direction, from the
			i-th square on the board.
	Moves is 64 long; moves[i] is variable length delimited by NULL; and
	moves[i][j] is variable length delimited by the value of i.
*/

initmoves()
{
static    int i,
		j,
		k,
		l,
		m,
		n;

#ifndef NONDYNAMIC
    void *calloc();
#endif
    register boardchar **pointers,
		 *bytes;

#ifndef NONDYNAMIC
	/* 484 and 2016 are computed by rote */
    pointers=((boardchar **) calloc(484,sizeof(boardchar **)));
	if(pointers==NULL)
	{
        _printf("initmoves: cannot allocate pointers.\n");
        clearkey();
        mygetch();
		exit(0);
	}
	bytes=calloc(2016,sizeof(char));
	if(bytes==NULL)
	{
        _printf("initmoves: cannot allocate bytes.\n");
        clearkey();
        mygetch();
		exit(0);
	}
#else
    pointers=_space_pointers;
    bytes=_space_bytes;
#endif

#if DEBUG_LEVEL > 1
    _printf("pointers  set to  %d\n",((unsigned int) pointers));
    _printf("   bytes  set to  %d\n",((unsigned int) bytes));
#endif

	/* for each square on the board */
	for(i=0;i<8;i++) for(j=0;j<8;j++)
	{

		/* set the corresponding entry of moves to some free pointers */
		moves[i*8+j]=pointers;

		/* for each direction */
		for(k=(-1);k<2;k++) for(l=(-1);l<2;l++)
			if(k || l)	/* if neither k or l we aren't going anywhere */
			{
				*pointers=bytes; /* point to some free bytes */
				/* let m and n walk to the edge of the board */
				for(m=i+k,n=j+l;m>=0 && m<8 && n>=0 && n<8;m+=k,n+=l)
					(*bytes++)=m*8+n;
				if(m!=i+k || n!=j+l)
				/* then we managed to walk somewhere */
				{
					(*bytes++)=i*8+j;	/* terminate bytes list				*/
					pointers++;			/* get pointer for next direction	*/
				}
			}
		(*pointers++)=NULL;	/* terminate pointers list */
	}

#if DEBUG_LEVEL > 1
    _printf("pointers left at %d\n",((unsigned int) pointers));
    _printf("   bytes left at %d\n",((unsigned int) bytes));
#endif

}

#if DEBUG_LEVEL > 2
int check_moves()
{
  int i,
		chk1,
		chk2;

    register boardchar **ptr1,
		 *ptr2;

	chk1=chk2=0;
	for(i=0;i<64;i++)
		for(ptr1=moves[i];*ptr1;ptr1++)
			for(ptr2=(*ptr1);(*ptr2)!=i;ptr2++)
			{
				if(*ptr2<0 || *ptr2>63)
                    _printf("moves subscript out of range at i=%d sub=%d\n\7",i,*ptr2);
				chk2+=chk1^=(*ptr2);
			}
	return(chk1+chk2);
}

int count_boards()
{
     int i;

    register BOARD *ptr;

	for(i=0,ptr=freeboards;ptr;i++,ptr=ptr->sons[0]);
	return(i);
}
#endif

/*
	allsons finds all sons for the board which is its parameter, sets the
	array of pointers to sons to point to new boards containing the resulting
	boards, sets the val member of each son board structure to the value as
	evaluated by worth, and sorts the sons in order, to focus the alphabeta
	search. It returns the number of sons it found.
*/

int allsons(pos)
BOARD *pos;
{
static    int cur, /* son next to allocate */
		i;

static    char mine,          /* mine, from the point of the current player   */
		 hisn,			/* likewise										*/
         whose;         /* temporary variable--keep from recomputing    */
    register boardchar *board;        /* pointer into the board array                 */
static    boardchar ***move_ptr,   /* pointer into the moves array                 */
		 **dir_ptr,		/* pointer into moves[i] arrays of directions	*/
		 *sub_ptr;		/* pointer into moves[i][j] arrays of subscrtips*/

    BOARD *resultof();  /* create a board resulting from making a move  */
static    BOARD *curson;    /* point to current son                         */

    cur=0;

#if DEBUG_LEVEL > 1
	allsons_called++;
#endif

	mine=max?MINE:HIS;
	hisn=max?HIS:MINE;
	board=pos->array[0];

	/* for(i=0;i<64;i++) with move_ptr=moves[i] */
	for(i=0,move_ptr=moves;i<64;i++,move_ptr++)
	{
		/* if(board[i]==EMPTY) */
		if(!board[i])
			/* for(j=0;moves[i][j]!=NULL;j++) with sub_ptr=moves[i][j] */
			for(dir_ptr=(*move_ptr);sub_ptr=(*dir_ptr++);)
				/* if he owns the cell in the j-th direction */
				if(board[*sub_ptr++]==hisn)
				{
					/* scan until edge of board or end of list */
					/*
						NOTE: moves[i][j] is delimited by i, so the edge of
						the board looks like a cell containing the same thing
						as board[i], which must be empty if I got into this
						code; therefore, hitting edge of board looks just
						like seeing a space at the end of the string of
						opponents pieces, which means I cannot capture them.
					*/
					while((whose=board[*sub_ptr++])==hisn);
					if(whose==mine) /* then we have a possible capture */
					{
						curson=pos->sons[cur++]=resultof(pos,i);
						curson->val=(*worth)(curson);
						goto endit;	/* don't look in other directions */
					}
				}
endit:	;
	}

#if DEBUG_LEVEL > 0
	if(cur>MAXSONS)
	{
        _printf("allsons: I needed %d sons for",cur+1);
		putboard(pos,NULL);
        _printf("allsons: but I only am alotted %d.\n",MAXSONS);
        _printf("Sorry, boss.\n");
        clearkey();
        mygetch();
		exit(0);
	}
#endif
#if DEBUG_LEVEL > 1
	if(greatest_branching < cur)
		greatest_branching=cur;
#endif

	sort(pos->sons,cur);
	pos->sons[cur]=0;
	return(cur);
}

/*
	Resultof makes a copy of the board (using _move(), a fast block
	move routine that comes with Mark DeSmet's Cware C compiler, if
	the code is being generated for an IBM-PC) and flips all squares
	thet need to be flipped based on making a move at position x
	(where x ranges for 0 to 63), takes the square at x, and returns
	a pointer to the resulting board. It calls newboard(), the free
	board server.
*/

BOARD *resultof(father,x)
BOARD *father;
int x;
{
static     int mine,   /* mine, from the point of view of the current player   */
		hisn;	/* likewise												*/

    register boardchar *board;    /* pointer into the board array                     */
static   boardchar **dir, /* pointer for moves[x][j], a direction                 */
		 *sub;	/* pointer to a subscript, moves[i][j][k]				*/

    BOARD *newboard();
static  BOARD *temp;

	mine=max?MINE:HIS;
	hisn=max?HIS:MINE;
	temp=newboard();

	/* Copy the board. Use a block move on the PC	*/
    memcpy(temp->array[0],father->array[0],64);

	board=temp->array[0];

	/* for(j=0;moves[x][j]!=NULL;j++) */
	for(dir=moves[x];*dir;dir++)
		/* if the cell in the j-th direction is his  */
		if(board[**dir]==hisn)
		{
			sub=(*dir);

			/* scan as long as the pieces are his	*/
			/* (Please see discussion in allsons	*/
			while(board[*++sub]==hisn);

			/* if the search was terminated by a piece of mine */
			if(board[*sub]==mine)
			{
				/* do the same scan, flipping pieces as you go */
				sub=(*dir);
				while(board[*sub]==hisn)
					board[*(sub++)]=mine;
			}
		}

	/* put a piece where we actually moved */
	board[x]=mine;
	return(temp);
}

/*
	firstboard() returns a pointer to a board set up in the initial position.
	It is the only routine that actually zeros out a board array, and sets
	things up in it. The remainder of the boards are made by copying and
	then changing, by resultof(). The initial position is reversed if we
	are going first (because what is stored is not x's and o's, but MINE
	and HIS) and can be reversed again by a reverse switch.
*/

BOARD *firstboard()
{
    register BOARD *temp;
    BOARD *newboard();

    static int i,
		j;

	temp=newboard();

	/* zero out the array */
	for(i=0;i<8;i++)
		for(j=0;j<8;j++)
			temp->array[i][j]=EMPTY;
	/* put the start position into the cells */
	/* either gofirst or reversed can switch the initialization */
	temp->array[3][3]=temp->array[4][4]=(!(gofirst^reversed))?MINE:HIS;
	temp->array[3][4]=temp->array[4][3]=(!(gofirst^reversed))?HIS:MINE;

#if DEBUG_LEVEL > 1
	temp->val=0;
#endif

	return(temp);
}

/*
	Getmove gets a move from the user. If the computer is an IBM-PC,
	the result is immediately displayed; otherwise, the user must wait
	for the computer to move. This is because of timing considerations;
	dumping a board into the video ram takes very little time, while
	cramming it through a 1200 baud line takes a fair while.
	There is complete checking for invalid input, but no verify or revise
	option (typo's will hurt you if they are legal moves).
*/

getmove(board)
BOARD **board;
{
     static int i,
		j,
		k,
        n,ind;
     static int ch[2];
     static unsigned c;
     static int x=3,y=3;

/*    BOARD *resultof(); */
    register BOARD *temp;

	max=FALSE;

	/* if(<the number of sons found>==0) */
	if(!(n=allsons(*board)))
	{
        unsigned int c;
        movecursor(0,20);
        _printf("You can't");
        movecursor(1,20);
        _printf("move.");
        movecursor(2,20);
        _printf("My turn.");
        movecursor(3,20);
        _printf("Press");
        movecursor(4,20);
        _printf("return.");
        person_last_x=person_last_y=-1;

        clearkey();
        while((c=mygetch())!='\n' && c!=KEY_LOWER_ENTER &&
        c!=KEY_UPPER_ENTER && c!='\r');
        reset_term();
        putboard((*board),NULL);
#ifndef NOTIME
        time=time_tick();
#endif
		return;
	}

	movenumber++;
	if(n==1)
	{
        for(i=0;i<8;i++) for(j=0;j<8;j++)
            if( !((*board)->array[i][j]) && (*board)->sons[0]->array[i][j] )
            {
                person_last_x=j;
                person_last_y=i;
                goto BreakOut;
            }
BreakOut:
        xorothellocursor(person_last_y+YOFFSET,person_last_x+xoffset);

        movecursor(0,20);
        _printf("You have");
        movecursor(1,20);
        _printf("only one");
        movecursor(2,20);
        _printf("move.");
        movecursor(3,20);
        _printf("Press");
        movecursor(4,20);
        _printf("return.");

        clearkey();
        while((c=mygetch())!=KEY_LOWER_ENTER && c!='\n' && c!='\r' && c!=KEY_UPPER_ENTER);

#ifndef NOTIME
		time=time_tick();
#endif
		temp=(*board)->sons[0];

#if DEBUG_LEVEL > 1
		temp->val=worth(temp);
#endif

        reset_term();
        putboard(temp,NULL); /* (*board)); */

		reclaim(*board);
		(*board)=temp;
		return;
	}

	/* get and validity check move */
retry:
    movecursor(0,20);
    _printf("Your");
    movecursor(1,20);
    _printf("move?   \8\8");
    clearkey();
    ind=0;
    xorothellocursor(y+YOFFSET,x+xoffset);
    while(ind<2 && (c=mygetch())!=KEY_LOWER_ENTER && c!=KEY_UPPER_ENTER && c!='\r' &&
     c!='\n' && c!=' ')
    {
        switch(c)
        {
            case KEY_LEFT:
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         if(x>0) x--;
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         break;
            case KEY_RIGHT: xorothellocursor(y+YOFFSET,x+xoffset);
                         if(x<7) x++;
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         break;
            case KEY_UP: xorothellocursor(y+YOFFSET,x+xoffset);
                         if(y>0) y--;
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         break;
            case KEY_DOWN:
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         if(y<7) y++;
                         xorothellocursor(y+YOFFSET,x+xoffset);
                         break;
            default: ch[ind++]=c; termputch(c); break;
        }
    }
    if(ind==2)
    {
        if(ch[0]>='a' && ch[0]<='h')
        {
            int swap;
            swap=ch[0];
            ch[0]=ch[1];
            ch[1]=swap;
        }
        i=ch[0]-'0'-1;
        j=ch[1]-'a';  /* tolower() */
        if(i>=0 && i<8 && j>=0 && j<8)
        {
          xorothellocursor(y+YOFFSET,x+xoffset);
          xorothellocursor(i+YOFFSET,j+xoffset);
          x=j;
          y=i;
        }
    }
    else
    {
        i=y;
        j=x;
    }
    person_last_x=j;
    person_last_y=i;
    /* if(<position is out of range> || board[i][j]!=EMPTY) */
	if(i<0 || i>7 || j<0 || j>7 || (*board)->array[i][j])
	{
        movecursor(0,0);
        _printf("Try again!          \7");
        xorothellocursor(y+YOFFSET,x+xoffset);
        goto retry;
	}
	/* scan the sons list, looking for a son with this cell occupied	*/
	for(k=0;!((*board)->sons[k]->array[i][j]);)
		/* if we have reached the end of the list without finding one	*/
		if(++k==n)
		{
            movecursor(0,0);
            _printf("Try again!          \7");
            xorothellocursor(i+YOFFSET,j+xoffset);
            goto retry;
		}

    /* clean up stray sons */
	for(i=0;i<n;i++)
		if(i!=k)
			reclaim((*board)->sons[i]);
	temp=(*board)->sons[k];

#if DEBUG_LEVEL > 1
	temp->val=worth(temp);
#endif

    reset_term();
    putboard(temp,NULL); /* (*board)); */

	reclaim(*board);
	*board=temp;
}

/*
	Score simply adds up the number of cells owned by each player,
	reports the results, and leaves.
*/

score(board)
BOARD *board;
{
    static char i,
         sums[3];
    register boardchar *ptr;
    sums[0]=sums[1]=sums[2]=0;

	ptr=board->array[0];
	for(i=0;i<64;i++)
		sums[*ptr++]++;
    movecursor(0,0);
    _printf("I got %d; you got %d; %s\n",sums[MINE],sums[HIS],
			sums[MINE]>sums[HIS]?"I win":
            (sums[MINE]==sums[HIS]?"a draw":"you win"));
    clearkey();
    mygetch();
}


/*
	Putboard can be used for one or two boards. For one board, make the
	second parameter NULL. It calls a function fast_puts to put the string
	out. This is to allow quick board drawing on a PC.
	Optionally, error checking code can be compiled in, to check for the
	(common) C bug of wandering of into randomn memory locations.
	Throughout the function, realize the "if(new)" is functionally identical
	to "if(new!=NULL)".
*/

putboard(old,new)
BOARD *old,
	  *new;
{
    static int i,
		j,
		k;

    char s[80];

    movecursor(1,0);
	if(new)
        fast_puts(" abcdefgh  abcdefgh\n");
	else
        fast_puts(" abcdefgh\n");

	/* for every row */
	for(i=0;i<8;i++)
	{
		k=0;
        _sprintf(s,"%d",i+1);
		while(s[++k]);

		/* for every column in the first board */
		for(j=0;j<8;j++)
		{

#if DEBUG_LEVEL > 0
			if(old->array[i][j]<0 || old->array[i][j]>2)
			{
                _printf("putboard: garbage in cell %d %d--exiting...\n",i,j);
                clearkey();
                mygetch();
				exit(0);
			}
#endif
              s[k++]=print_chars[old->array[i][j]];
		}

		if(new)
		{
            _sprintf(s+k," %d",i+1);
			while(s[++k]);

			/* for every column in the second board */
			for(j=0;j<8;j++)
			{

#if DEBUG_LEVEL > 0
				if(new->array[i][j]<0 || old->array[i][j]>2)
				{
                    _printf("putboard: garbage in cell %d %d--exiting...\n",i,j);
                    clearkey();
                    mygetch();
					exit(0);
				}
#endif

                s[k++]=print_chars[new->array[i][j]];
			}

		}
        if(i!=7) s[k++]='\n';
		s[k]='\0';
		fast_puts(s);
	}
    if(new) xoffset=11; else xoffset=1;
    if(person_last_x!=-1)
      putbox(person_last_y+YOFFSET,person_last_x+1);
    if(new && pc_last_x!=-1)
      putbox(pc_last_y+YOFFSET,pc_last_x+11);
#if DEBUG_LEVEL > 1
	if(new)
        _sprintf(s,"                  %d                                     %d\n",
				old->val,new->val);
	else
        _sprintf(s,"                  %d\n",old->val);
	fast_puts(s);
#endif
#ifdef STACK_TRACK
    ozputs(0,0,itoa((int)lowest_stack));
#endif
}




