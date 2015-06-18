#include "othello.h"


/*
	Worth_1 is the worth function containing all the subtle strategic
	heuristic information in the game. It is used until we can start
	looking all the way to the end of the game, at which point the
	optimum heuristic function, h* becomes available. That is called
	worth_2, and is much simpler.
*/

worth_2(board)
BOARD *board;
{

    static int valsum[3];

    register boardchar *t;

    valsum[0]=valsum[1]=valsum[2]=0;

#if DEBUG_LEVEL > 1
	worth_called++;
#endif

    t=board->array[0];
	/*
		The following 64 lines could have been replaced with
			for(i=0;i<64;i++)
				valsum[board[i]]++;

		but it would have been slower.
	*/
	valsum[(t[0])]++;
	valsum[(t[1])]++;
	valsum[(t[2])]++;
	valsum[(t[3])]++;
	valsum[(t[4])]++;
	valsum[(t[5])]++;
	valsum[(t[6])]++;
	valsum[(t[7])]++;
	valsum[(t[8])]++;
	valsum[(t[9])]++;
	valsum[(t[10])]++;
	valsum[(t[11])]++;
	valsum[(t[12])]++;
	valsum[(t[13])]++;
	valsum[(t[14])]++;
	valsum[(t[15])]++;
	valsum[(t[16])]++;
	valsum[(t[17])]++;
	valsum[(t[18])]++;
	valsum[(t[19])]++;
	valsum[(t[20])]++;
	valsum[(t[21])]++;
	valsum[(t[22])]++;
	valsum[(t[23])]++;
	valsum[(t[24])]++;
	valsum[(t[25])]++;
	valsum[(t[26])]++;
	valsum[(t[27])]++;
	valsum[(t[28])]++;
	valsum[(t[29])]++;
	valsum[(t[30])]++;
	valsum[(t[31])]++;
	valsum[(t[32])]++;
	valsum[(t[33])]++;
	valsum[(t[34])]++;
	valsum[(t[35])]++;
	valsum[(t[36])]++;
	valsum[(t[37])]++;
	valsum[(t[38])]++;
	valsum[(t[39])]++;
	valsum[(t[40])]++;
	valsum[(t[41])]++;
	valsum[(t[42])]++;
	valsum[(t[43])]++;
	valsum[(t[44])]++;
	valsum[(t[45])]++;
	valsum[(t[46])]++;
	valsum[(t[47])]++;
	valsum[(t[48])]++;
	valsum[(t[49])]++;
	valsum[(t[50])]++;
	valsum[(t[51])]++;
	valsum[(t[52])]++;
	valsum[(t[53])]++;
	valsum[(t[54])]++;
	valsum[(t[55])]++;
	valsum[(t[56])]++;
	valsum[(t[57])]++;
	valsum[(t[58])]++;
	valsum[(t[59])]++;
	valsum[(t[60])]++;
	valsum[(t[61])]++;
	valsum[(t[62])]++;
	valsum[(t[63])]++;

	/* return((<number I have> - <number he has>) * 2); */
	return((valsum[1]-valsum[2])<<2);
}

