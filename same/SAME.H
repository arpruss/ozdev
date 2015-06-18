
#ifndef DEFINE_VARS
# define E extern
#else
# define E
#endif

#define CURSOR_SPEED 16
#define DEF_SIZE 1
#define DEF_NUMBER 2
#define DEF_SLOW 1
#define DEF_QUIET 0
#define MAXWIDTH 24
#define MAXHEIGHT 10
#define EMPTY 0
#define MAXPIECES 5
#define PIECESRANGE (MAXPIECES-1)
#define NUMSIZES 5
#define GREY_MASK 0x80
#define PIECE_SIZE 8
#define INFO_X ((MAXWIDTH)*PIECE_SIZE+3)
#define MAXNAMELEN 8

E unsigned highscores[NUMSIZES][PIECESRANGE];
E char highscore_names[NUMSIZES][PIECESRANGE][MAXNAMELEN+1];
E byte board[MAXWIDTH+2][/* MAXHEIGHT+2 */ 16];  /* 16 should be faster! */
E byte width;
E byte height;
E byte numpieces;
E long score;
E byte rest;
E byte y0;
E byte cursor_col,cursor_row;
E byte changed;
extern byte *pieces[MAXPIECES+1];
extern byte selector[];
extern byte cursor[];
extern byte left_line[];
extern byte current_size;
extern byte current_number;
extern byte current_slow;
extern byte current_quiet;

void clearshade(void);
unsigned shade(byte i0,byte j0);
unsigned quickshade(byte _i0,byte _j0);
void showboard(void);
void greyputpiece(byte x,byte y,byte piece);
void clearcursor(byte i,byte j);
void showcursor(byte i,byte j);
void _ozputsprite0(unsigned offset, byte height, byte *src);
void showcount(unsigned selected);
void hide_selection(void);
byte check_for_moves(void);
void showscore(void);
void showrest(void);
long play(void);
int menu(void);
void save_stuff(void);
void load_stuff(void);
void help(void);
unsigned cursor_and_key(byte i,byte j);
void findmove(byte *iptr,byte *jptr);

