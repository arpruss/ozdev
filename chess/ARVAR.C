#ifdef HEADER
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern int port;
#define EXTERN extern
#else
#ifdef DEBUG
int port=1;  /* used for debuging.  Where to send output.*/
             /* 1=stdout,  2=stderr                      */
#endif
#define EXTERN
#endif

/* The 6809 has a fast access memory area called the direct page.
   It is a 256 byte region that can be accessed much faster than
   either stack or regular static vars.  Not all microprocessors
   have a fast access memory region, so if yours doesn't, just
   uncomment the following #define
*/
#define direct

#define MAXDEPTH      20    /* 32    /* maximum search depth                */
#define MAXLEVEL      16    /* maximum user set search level                */
#define BIGNUM        15000 /* BIGNUM is just a very large number.  It MUST */                             /* NOT be the absolute maximum or it will over  */
                            /* flow                                         */
#define MATEVAL       14500 /* 500 points less than bignum                  */
#define MAXNONMATE    14000 /* the maximum non mate score possible          */
#define MAXMOVES       512  /* maximum number of moves stored during game   */
                            /* (MAXLEVEL * 32) + (MAXDEPTH-MAXLEVEL)*8      */
                            /* should be enough for search & cap search     */
#define MAXHIST        106  /* maximum number of moves in a game : cannot exceed 255 */
#define SHIFT_HIST_BY  4    /* amount to shift the game history by: */
                            /* MAXHIST-SHIFT_HIST_BY should be at least 100 */
#define MAXPLYMVS      250  /* max num of moves per ply.  used in vermove   */

/* a few useful defines */
#define TRUE  1
#define YES   1
#define FALSE 0
#define NO    0
#if 0
#define NULL  0
#define EOF  -1   /* the End Of File code */
#endif

/* pieces */
#define EMPTY   0
#define PAWN    1
#define KNIGHT  2
#define BISHOP  3
#define ROOK    4
#define QUEEN   5
#define KING    6

/* color */
#define BLACK   2
#define WHITE   0
#define NONE    1

#define PAWNVAL     100
#define KNIGHTVAL   330
#define BISHOPVAL   355
#define ROOKVAL     550
#define QUEENVAL   1100
#define KINGVAL    1500

/* Types of moves (bits 0-7) */
/* bits 0-2 indicate promotion piece type */
#define PROMOTION      8     /* a promotion */
#define ENPASSANT     16     /* an enpassant capture */
#define CASTLE        32     /* a castling move */
#define PROMTHRT      64     /* a pawn threating to promote */
#define CAPTURE      128     /* a capture move */

/* Misc info about the move, score and evaluation (bits 8-15) */
#define STANDPAT     256     /* standing pat in capture search */
#define REPETITION   512     /* draw by repetition */
#define STALEMATE   1024     /* draw by stalemate */
#define CHECKMATE   2048     /* checkmate */
#define CHECK       4096     /* check */
#define FULLEVAL    8192     /* a full eval was done */
#define FULLSRCH   16384     /* a full search was done */
#define EXACT      32768     /* the score is exact */

/* a few 'routines' */
#define getrow(s)    ((uchar)((uchar)(s)>>3))
#define getcol(s)    ((uchar)((uchar)(s)&7))
#define chngcolor(c) (2-(c))
#define odd(v)       (uchar)((uchar)(v)&(uchar)1)
/* #define copymv(t,f)  memcpy((void*)(t),(void*)(f),sizeof(movetype)) */
void copymv(void *t,void *f);

/* types */
typedef unsigned char uchar;
#define bool   uchar   /* boolean */
#define tiny   char   /* a small signed int */

typedef struct {
                tiny f,t;
                int score;
                unsigned flags;
                } movetype;

typedef struct {tiny f,t;
                uchar /* unsigned */ flags; /* only need lower 8 bits of flags */
                uchar /* int */ game50;
                uchar piece,captured;
                uchar color;
/*
                unsigned piece:3;
                unsigned captured:3;
                unsigned color:2;
*/
                tiny epsquare;} gametype;

typedef struct {unsigned char year,month,day,hour,minute,second;} timetype;


void      scorepawns(void);
int       evalu8(movetype *move,int prevpscr,uchar side,
                 int lowbound,int hibound,uchar depth,bool *incheck,uchar qsearch);
void      updateweights(uchar depth);
movetype *gencaps(movetype *buffer,uchar color);
movetype *genmoves(movetype *buffer,uchar color);
movetype *genlegal(movetype *buffer,uchar color);
bool      sqatakd(uchar sq,uchar side);
void      findpieces(void);
void      mvenglish(movetype *mv,char *st);
void      checkstatus(uchar player);
bool      mveq(movetype *mv1,movetype *mv2);
void      processmove(movetype *mv);
void      makemove(movetype *mvmv);
void      unmakemove(void);
void      sort(movetype *first,movetype *last);
void      entermoves(void);
void      undo(void);
void      inputcommand(void);
void      clearboard(void);
void      newboard(void);
void      newgame(void);
void      initgame(void);
int       cmdis(char *str);
void      readcmd(void);
bool      verifymove(void);
void      gentime(void);
void      captime(void);
void      evaltime(void);
void      printboard(void);
void      setboard(void);
void      help(void);
int       search(movetype *prevmove,movetype *buffer,
                 int lowbound,int hibound,uchar color,
                 uchar depth,uchar ext,uchar qlevel,movetype *bstline);
void      initselect(void);
void      updatewindow(void);
void      selectmove(int player);
int       setmate(movetype *move,uchar side,uchar depth);
int       repetition(void);
void      pvsenglish(movetype *pvs);
void      scalehistory(void);
void      searchtime(int research,int depth,int bstscore,
                     int lowbound,int avgscore,int zwindow);
void      selected(void);
void      unflag(movetype *mvlist,movetype *last,bool reset);
void      initprg(void);
void      clrscrn(void);
void      clreoln(void);
void      Bell(void);
void      resetclock(void);
void      elaptime(void);
#define outchar termputch
#define inchar ozgetch
#define FlushIO()

unsigned ozgetch(void);
void termputch(char c);

#if 0
/*
** The standard library routines
*/
int       abs(int i);
void      memcpy(char *dest,char *src,int len);
int       atoi(char *s); /* decode an integer */
void      itoa(int n,char *s);
int       isdigit(int c);
int       isprint(int c);
int       toupper(int c);
int       tolower(int c);
void      revstr(char *s);
char     *strcpy(char *dest, char *source);
char     *strcat(char *first, char *second);
int       strlen(char *str);
int       readln(char *str,int size);
/*void      printf(char *fmt);*/
void      printf(char *fmt,...);
#endif





/* The following vars are the 'position representation' vars */
EXTERN uchar /* tiny */   board[64];      /* the board */
EXTERN uchar /* tiny */  brdcolor[64];   /* the color of the squares */
EXTERN bool   castled[3];     /* castled flags */
EXTERN uchar  computer;       /* the color the computer is playing */
EXTERN int    epsquare;       /* the enpassant square */
EXTERN uchar  human;          /* the color the human is playing */
EXTERN tiny   kingloc[3];     /* location of kings */
EXTERN int    mtl[3];         /* material for each side */
EXTERN int    mvboard[64];    /* the move count board */
EXTERN uchar /* tiny */   pawncnt[3];     /* pawn count for each side */
EXTERN uchar /* tiny */   pawncol[3][8];  /* num of pawns on each column */
EXTERN uchar tomove;         /* color of the side to move */

/* The following are various statistics gathered during search */
#ifdef DEBUG
EXTERN unsigned capnodes;     /* the num of capture nodes searched */
EXTERN unsigned evals;        /* the number of full evalu8s done */
EXTERN unsigned pawnevals;    /* the num of pawn evaluations */
EXTERN unsigned nodes;        /* the num of nodes search */
EXTERN unsigned nbp;          /* the number of bottom pos reached */
EXTERN unsigned extcount;     /* number of search extensions done */
EXTERN unsigned maxdepth;     /* max depth reached during search */
#endif

/* The following are concerned about search time control / level */
EXTERN unsigned    ElapClock;  /* elapsed time in seconds */
EXTERN direct int  extratime;  /* extra time for search */
EXTERN direct uchar level;      /* iterative level of search */
EXTERN uchar playlevel;  /* the level to search (not time control) */
EXTERN direct unsigned responsetime;/* time level of search wanted */
EXTERN direct bool tcontrol;   /* time control flag (or ply cntrl search) */
EXTERN direct bool timeout;    /* out of search time? */

/* The following are various user options */
EXTERN bool     bothsides;      /* computer plays both sides */
/* EXTERN bool     pvsflag;        /* print the pvs? */
#define pvsflag 0
EXTERN bool     qsearch;        /* toggle the quiscience (capture) search*/
EXTERN bool     reverse;        /* reverse board display */

/* The following are various game status vars */
/* EXTERN bool     breakpress;     /* break pressed flag */
EXTERN bool     draw;           /* drawn game flag */
EXTERN uchar /* int */      game50;         /* starting movenum for the 50 move rule */
EXTERN gametype gamehist[MAXHIST+MAXDEPTH]; /* the move history */
EXTERN direct gametype *gameptr;/* a pointer to next gamehist[]. */
EXTERN bool     mate;           /* mate flag */
EXTERN uchar /* int */     movenum;        /* the move number of the game */
EXTERN int      shifted_gamehist; /* number of times gamehist was shifted */

/* The following are misc. vars used mainly in the I/O routines */
EXTERN char     cmdline[20];    /* the cmd line read from the keyboard */
EXTERN int      estscore;       /* the estimated score after the search */
/* EXTERN unsigned history[4096];  /* the history heuristic */
EXTERN char     msg[80];        /* a message from the computer */
EXTERN movetype pvs[MAXDEPTH];  /* the vest move to make at each level */
/* EXTERN char     pvsmsg[200];    /* the pvs in english */
EXTERN char     status[80];     /* a status message (mate etc) */


/* initialized data */
/* The following are constant and can be placed in the program segment */
#ifdef HEADER

EXTERN int   *pdir[];         /* the pointer to the move offsets */
EXTERN /* tiny */ uchar   map[64];        /* map 8x8 to 10x12 board */
EXTERN char   name[14];       /* name of the pieces */
EXTERN int    pieceval[8];    /* piece val */
#if 0
EXTERN bool   sweep[7];       /* sweep piece? */
#endif
EXTERN tiny   unmap[120];     /* map 10x12 to 8x8 board */
EXTERN char archess_version[];


#else
char archess_version[]="AR-Ches\xF3""v1.1\n";

/* move generation data */
#if 0
bool   sweep[7]={FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,FALSE}; /* sweep piece? */
#endif
static int qmvs[]={1,10,-1,-10,9,11,-9,-11,0,0};   /* queen move offsets  */
static int rmvs[]={1,10,-1,-10,0,0};               /* rook move offsets   */
static int bmvs[]={9,11,-9,-11,0,0};               /* bishop move offsets */
static int nmvs[]={8,-8,12,-12,19,-19,21,-21,0,0}; /* knight move offsets */
static int wpmvs[]={9,11,8,16};                /* white pawn move offsets */
static int bpmvs[]={-9,-11,-8,-16};            /* black pawn move offsets */
int *pdir[]={bpmvs,wpmvs,nmvs,bmvs,rmvs,qmvs,qmvs};

char name[]=" PNBRQK";    /* name of the pieces */
    /* piece val */
int  pieceval[8]={0,PAWNVAL,KNIGHTVAL,BISHOPVAL,ROOKVAL,QUEENVAL,KINGVAL,0};

/* map our 8x8 board onto a 10x12 so we can check for border */
uchar /* tiny */ map[64]=
   { 21, 22, 23, 24, 25, 26, 27, 28,
     31, 32, 33, 34, 35, 36, 37, 38,
     41, 42, 43, 44, 45, 46, 47, 48,
     51, 52, 53, 54, 55, 56, 57, 58,
     61, 62, 63, 64, 65, 66, 67, 68,
     71, 72, 73, 74, 75, 76, 77, 78,
     81, 82, 83, 84, 85, 86, 87, 88,
     91, 92, 93, 94, 95, 96, 97, 98};

/* map the imaginary 10x12 board back onto our 8x8 board */
tiny unmap[120]=
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1, 0, 1, 2, 3, 4, 5, 6, 7,-1,
   -1, 8, 9,10,11,12,13,14,15,-1,
   -1,16,17,18,19,20,21,22,23,-1,
   -1,24,25,26,27,28,29,30,31,-1,
   -1,32,33,34,35,36,37,38,39,-1,
   -1,40,41,42,43,44,45,46,47,-1,
   -1,48,49,50,51,52,53,54,55,-1,
   -1,56,57,58,59,60,61,62,63,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

#endif

/*
   If you want to add save/load game functions you'll need to save:
   board, brdcolor, castled, computer, draw, epsquare, game50, gamehist,
   human, kingloc, level, mate, movenum, mvboard, playlevel, pvsflag,
   qsarch, responsetime, reverse, tcontrol, tomove, font.

*/
#define HISTORY

#ifdef HISTORY
#define readhistory ozreadauxword2
unsigned readhistory(unsigned x);
uchar addhistory(unsigned x,unsigned y);
#else
#define scalehistory()
#endif

extern uchar reversevideo;
unsigned gettime(void);
uchar taxicab(uchar x,uchar y);
uchar distance(uchar x,uchar y);
extern uchar doxor;
extern uchar curcol,currow;
EXTERN bool movepieces;
EXTERN bool font;
void termputch(char c);
void putbitmap(uchar x,uchar y,uchar h, uchar *bitmap);
extern uchar font8x8[];
EXTERN uchar smallboard;
EXTERN uchar doblank;
#define XORHL 0xAE
#define ORHL 0xB6
#define NOP  0
#define SCREENX(col) (10+(col)*10)
#define SCREENY(row) ((row)*10)
#define SCREENROW(row) (reverse?(row):7-(row))
#define SCREENCOL(col) (reverse?7-(col):(col))
EXTERN uchar breakctrl;
EXTERN uchar doenter;
#define ozgetch ozgetchblank
