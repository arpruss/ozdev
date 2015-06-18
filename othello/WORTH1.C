#include "othello.h"


/*
	Worth_1 is the worth function containing all the subtle strategic
	heuristic information in the game. It is used until we can start
	looking all the way to the end of the game, at which point the
	optimum heuristic function, h* becomes available. That is called
	worth_2, and is much simpler.
*/

worth_1(board)
BOARD *board;
{
    static int valsum[3]; /* buckets in which to accumulate the sums       */

/* The "unsigned" in the next three lines is due to ARP */
    register unsigned char *t;  /* temporary pointers for walking through the array */
    static unsigned char
		val1[3]={30,0,0},	/* value of (1,1) if (0,0)=0,1,2 */
		val2[3]={ 4,0,0},	/* value of (1,2) if (0,2)=0,1,2 */
		val3[3]={ 5,0,0};	/* value of (1,3) if (0,3)=0,1,2 */

#define ev0	50	/* value of pos 00 (corner)		*/
#define ev1	4	/* value of pos 01				*/
#define ev2	16	/* value of pos 02				*/
#define ev3	12	/* value of pos 03				*/
#define sv	20	/* value of a split on the edge */

/*
	 50,  4, 16, 12, 12, 16,  4, 50,
	  4,-30, -4, -5, -5, -4,-30,  4,
	 16, -4,  1,  0,  0,  1, -4, 16,		This is what the board cell
	 12, -5,  0,  0,  0,  0, -5, 12,		worth function looks like,
	 12, -5,  0,  0,  0,  0, -5, 12,		discounting all dynamic
	 16, -4,  1,  0,  0,  1, -4, 16,		dependancies.
	  4,-30, -4, -5, -5, -4,-30,  4,
	 50,  4, 16, 12, 12, 16,  4, 50
*/

/*
	f[] is a finite state automaton used for edge strategy
	It recognizes two related phenomena, which we call "splits";
	positions where two pieces owned by one player on an edge are

		a) separated by exactly 1 blank, or
		b) separated by 1 or more of the opponent's

	Invented by John White, it is structured as follows:

	f[105] can be viewed as 5 tiers of 7 states, each of which requires
	3 cells for the three possible input values.
	Starting at one corner, you scan along an edge.
	Keep always in mind that the values of board cells are

						0		EMPTY
						1		MINE
						2		HIS

	The states are numbered as follows:

	  state			decription						board
	  -----	----------------------------------		------

		0	currently scanning through blanks		... 0
		1	just seen a square of mine				... 1
		2	just seen a square if his				... 2
		3	a blank following a square of mine		... 10
		4	a blank following a square of his		... 20
		5	one of his following one of mine		... 12
		6	one of mine following one of his		... 21

	The following table identifies the transitions between states.
	The numbers down the left, labelling the rows, are the input
	cell values. The numbers across the top are state numbers.
	The contents of a cell in this matrix is the number of the
	state the will be result from the input at the left from the
	state above.

			0		1		2		3		4		5		6
		---------------------------------------------------------
	0	|	0	|	3	|	4	|	0	|	0	|	4	|	3	|
		---------------------------------------------------------
	1	|	1	|	1	|	6	|	1 -	|	1	|	6 -	|	6	|
		---------------------------------------------------------
	2	|	2	|	5	|	2	|	2	|	2 +	|	5	|	5 +	|
		---------------------------------------------------------

	The cells containing postfix "+" or "-" symbols represent
	situations in which we have found one of the key patterns,
	at which point we go to the state indicated in the immediately
	higher or lower tier, respectively. The final value is
	simply the number of the tier we are on at the end, multiplied
	by sv, the split value. Note that each state takes three array elements,
	so the actual contents of the array are three times the state
	numbers shown above, repeated 5 times, offset by the tier start
	subscripts, with special offsets applied to the cells which
	jump tiers. The array v[] is used for the last lookup, since
	we are uninterested in the exact state, but just the tier, and
	since the tier number must be multiplied by sv, we put the resulting
	products in v[]. Finally, the tiers are organized as follows:

			  offset	meaning			    value
			  ------ ----------------	    -----
		
				 0		 neutral			   0
				21		  good				  sv
				42		  bad				 -sv
				63		very good			 2*sv
				84		very bad			-2*sv

	With this organization, if the state of the machine at time t0 is
	S0 and the input is I0, the state at time t1 is f[S0+I0], and therefore
	the state at time t2 is f[f[S0+I0]+I1] and so forth.
	So, without further ado:
*/

	static char f[105]=
				{
/*----------------------------------------------------------------------------
|state    0		 	1		   2		 3		   4		  5			 6   |
|input 0  1  2   0  1  2   0   1  2   0  1  2   0  1  2   0   1  2   0   1  2|
----------------------------------------------------------------------------*/
	   0, 3, 6,  9, 3,15, 12, 18, 6,  0,45, 6,  0, 3,27, 12, 60,15,  9, 18,36,
	  21,24,27, 30,24,36, 33, 39,27, 21, 3,27, 21,24,69, 33, 18,36, 30, 39,78,
	  42,45,48, 51,45,57, 54, 60,48, 42,87,48, 42,45, 6, 54,102,57, 51, 60,15,
	  63,66,69, 72,66,78, 75, 81,69, 63,24,69, 63,66,69, 75, 39,78, 72, 81,78,
	  84,87,90, 93,87,99, 96,102,90, 84,87,90, 84,87,48, 96,102,99, 93,102,57
				};

/* v is the final pass of f, and is board value instead of next state */

	static int v[105]=
			   {
		0,0,0,0,0,0,0,0,0,0,-sv,0,0,0,sv,0,-sv,0,0,0,sv,
		sv,sv,sv,sv,sv,sv,sv,sv,sv,sv,0,sv,sv,sv,2*sv,sv,0,sv,sv,sv,2*sv,
		-sv,-sv,-sv,-sv,-sv,-sv,-sv,-sv,-sv,-sv,-2*sv,-sv,-sv,-sv,0,-sv,
				-2*sv,-sv,-sv,-sv,0,
		2*sv,2*sv,2*sv,2*sv,2*sv,2*sv,2*sv,2*sv,2*sv,2*sv,sv,2*sv,2*sv,
				2*sv,2*sv,2*sv,sv,2*sv,2*sv,2*sv,2*sv,
		-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,
				-2*sv,-2*sv,-2*sv,-sv,-2*sv,-2*sv,-2*sv,-2*sv,-2*sv,-sv
			   };

#if DEBUG_LEVEL > 1
	worth_called++;
#endif

	*valsum=valsum[2]=0;	/* clean out buckets	*/
    t=board->array[0];  /* set up pointers      */

	/* and let the finite state automoton roll--one edge in each term */
	valsum[1]=
      v[f[f[f[f[f[f[f[  *t ]+t[ 1]]+t[ 2]]+t[ 3]]+t[ 4]]+t[ 5]]+t[ 6]]+t[ 7]];
    valsum[1]+=
      v[f[f[f[f[f[f[f[  *t ]+t[ 8]]+t[16]]+t[24]]+t[32]]+t[40]]+t[48]]+t[56]];
    valsum[1]+=
     v[f[f[f[f[f[f[f[t[ 7]]+t[15]]+t[23]]+t[31]]+t[39]]+t[47]]+t[55]]+t[63]];
    valsum[1]+=
      v[f[f[f[f[f[f[f[t[56]]+t[57]]+t[58]]+t[59]]+t[60]]+t[61]]+t[62]]+t[63]];

	/*
		if the worth array shown in the comment above actually existed, the
		next 60 or so lines might have been written

			for(i=0;i<64;i++)
				valsum[board[i]]+=worth[i];

		but it doesn't, except in the defined constants ev0 through ev3.
		Besides, it's quicker to execute 60 statements than to go through
		a loop 60 times (no loop control and branching) and this function
		is at the bottom of the recursion, and needs to be fast.
	*/

	valsum[(t[0])]+=ev0;
	valsum[(t[1])]+=ev1;
	valsum[(t[2])]+=ev2;
	valsum[(t[3])]+=ev3;
	valsum[(t[4])]+=ev3;
	valsum[(t[5])]+=ev2;
	valsum[(t[6])]+=ev1;
	valsum[(t[7])]+=ev0;

	valsum[(t[8])]+=ev1;
	valsum[(t[9])]-=val1[*t];
	valsum[(t[10])]-=val2[t[2]];
	valsum[(t[11])]-=val3[t[3]];
	valsum[(t[12])]-=val3[t[4]];
	valsum[(t[13])]-=val2[t[5]];
	valsum[(t[14])]-=val1[t[7]];
	valsum[(t[15])]+=ev1;

	valsum[(t[16])]+=ev2;
	valsum[(t[17])]-=val2[t[16]];
	valsum[(t[18])]++;
	valsum[(t[21])]++;
	valsum[(t[22])]-=val2[t[23]];
	valsum[(t[23])]+=ev2;

	valsum[(t[24])]+=ev3;
	valsum[(t[25])]-=val3[t[24]];
	valsum[(t[30])]-=val3[t[31]];
	valsum[(t[31])]+=ev3;

	valsum[(t[32])]+=ev3;
	valsum[(t[33])]-=val3[t[32]];
	valsum[(t[38])]-=val3[t[39]];
	valsum[(t[39])]+=ev3;

	valsum[(t[40])]+=ev2;
	valsum[(t[41])]-=val2[t[40]];
	valsum[(t[42])]++;
	valsum[(t[45])]++;
	valsum[(t[46])]-=val2[t[47]];
	valsum[(t[47])]+=ev2;

	valsum[(t[48])]+=ev1;
	valsum[(t[49])]-=val1[t[56]];
	valsum[(t[50])]-=val2[t[58]];
	valsum[(t[51])]-=val3[t[59]];
	valsum[(t[52])]-=val3[t[60]];
	valsum[(t[53])]-=val2[t[61]];
	valsum[(t[54])]-=val1[t[63]];
	valsum[(t[55])]+=ev1;

	valsum[(t[56])]+=ev0;
	valsum[(t[57])]+=ev1;
	valsum[(t[58])]+=ev2;
	valsum[(t[59])]+=ev3;
	valsum[(t[60])]+=ev3;
	valsum[(t[61])]+=ev2;
	valsum[(t[62])]+=ev1;
	valsum[(t[63])]+=ev0;

	return(valsum[1]-valsum[2]);
}

/*
	If worth_2 is being called, we are looking all the way to the end of
	the game, and we can use the final board as an evaluator: the worth
	of the board is the number of squares more I have than he. This number
	is shifted left 2 to attempt to make the range of values returned
	approximately comparable to worth_1, so that worth_2 can be used
	by alphabeta when we find an early game ending.
*/

