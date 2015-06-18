#define TRUE  1
#define FALSE 0
#define WORST -1000
#define BEST   1000
#define MAXSONS 30
#define EMPTY '\0'
#define MINE  '\1'
#define HIS   '\2'

typedef unsigned char boardchar;

typedef struct boardtype
{
    struct boardtype *sons[MAXSONS];    /* pointers to descendants  */
    int val;                            /* worth of board position  */
    boardchar *array[8];                     /* the board itself         */
}
BOARD;

int worth_1(BOARD *board);
int worth_2(BOARD *board);
int allsons(BOARD *pos);
int alphabeta(BOARD *current,int depth,int alpha,int beta);
