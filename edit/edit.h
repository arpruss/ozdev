/* #define FIXED /* fixed width only */
#define TRANSL /* special character translation */

unsigned long oztime_fake(void);

#define MAX_LINES_PER_SCREEN 20

#define LINE_BUF_LEN 124
#define MAX_FILE_LENGTH 16384
#define WIDTH 239

#ifndef FIXED
extern byte NUM_FONTS;
extern byte font2;
extern byte font3;
#else /* FIXED */
#define NUM_FONTS 1
#endif /* FIXED */

     /* note: LINE_BUF_LEN does not divide MAX_FILE_LENGTH, and this
      matters for main.c */

extern byte selection_on;
extern unsigned selection_start;
extern unsigned selection_end;
extern unsigned selection_anchor;

/* from wrap.as */
extern unsigned file_length;
extern unsigned wrapped_length;
extern char saved_line[LINE_BUF_LEN];

unsigned search(unsigned start,byte l);
clear_wrap_to_end(unsigned offset);
unsigned wrap_line(unsigned offset);
unsigned grab_line(unsigned offset);
void insert(unsigned offset,unsigned length);
void delete(unsigned offset,unsigned length);
unsigned find_next_line(unsigned offset);
unsigned find_prev_line(unsigned offset);
void clear_area(unsigned start, unsigned count);

/* from edit.c */
void cursor_on(void);
void cursor_off(void);
extern unsigned last_found;
extern unsigned in_clipboard;
#ifndef FIXED
extern byte line_height;
extern byte lines_per_screen;
extern byte font_narrow_height;
extern unsigned line_height_bytes;
#else
#define line_height 6
#define font_narrow_height 6
#define lines_per_screen 13
#define line_height_bytes (30*line_height)
#endif

extern byte modified;
extern unsigned cur_offset;
extern unsigned screen_offset;
extern unsigned cur_line_offset;
extern byte ideal_x,display_x,y;
extern byte font;

extern char filename[];

/* from font.c */
#ifdef FIXED
#define FIXED_WIDTH 4
#endif

/* from show.c */
extern unsigned queued_scroll_up,queued_scroll_down;
extern byte changed[];
extern byte refresh;
unsigned show_line(byte n, unsigned offset);
void show_screen(unsigned offset);
void clear_rest(byte i);
void refresh_on(void);
void refresh_off(void);

#ifdef USE_MAIN
/* from main.c */
extern char directory[];
int write_file(void);
void read_file(void);
void edit(void);
#endif

/* from menu.c */
void menu(void);

/* from scr_nc.as */
void ttyprintstring(char *s);
extern byte curcol,currow;

/* from readidx.c */
#define MAX_MEMO_LENGTH 1956
#define DATE_OFFSET 13
#define TITLE_LENGTH 20
#define TITLE_OFFSET (DATE_OFFSET+9)
#define SPECIAL_LENGTH 6
#define MEMO_DATA_OFFSET (TITLE_OFFSET+TITLE_LENGTH+1)
#define HEADER MEMO_DATA_OFFSET
#define clipboard (memo+HEADER)
#define MAX_CLIPBOARD (MAX_MEMO_LENGTH+1)
#define MAX_INDEX_COUNT 0x7F
#define PRELOAD_LEN 5
#define PRELOAD_NUM 19  /* (LEN+1)*NUM cannot exceed 115 bytes */
extern char _ozspare1start[PRELOAD_NUM][PRELOAD_LEN+1];
#define preload_titles _ozspare1start
/* extern char preload_titles[PRELOAD_NUM][PRELOAD_LEN+1]; */
 /* this is loaded in the spare1 area, and with the current 32K.AS
    cannot exceed 121 bytes */
extern char special_header[];
extern char special_date[];
extern unsigned start_extended;
extern unsigned total_memos;
extern char title[];
extern unsigned goto_recnum;
extern byte quick_index;
extern byte index_count;
extern union
{
    unsigned il[MAX_INDEX_COUNT];
    byte wt[256];
} iloc_and_wt;
#define width_table ((iloc_and_wt).wt)
#ifdef FIXED
# define width(i) 4
#else
# define width(i) (width_table[(byte)(i)])
#endif
#define index_locs ((iloc_and_wt).il)

extern byte index_lengths[MAX_INDEX_COUNT];
extern char line_buf[LINE_BUF_LEN];
/* extern byte index_lengths[MAX_INDEX_COUNT];
extern char line_buf[LINE_BUF_LEN];
extern unsigned index_locs[MAX_INDEX_COUNT];
extern byte width_table[128]; */

extern char preload_titles[PRELOAD_NUM][PRELOAD_LEN+1];
#define PRELOAD_POS(i)     ( start_extended/(PRELOAD_NUM+1) *((i)+1) )

/* from memoedit.c */
#define SEARCH_STRING_MAX 40
extern char search_string[SEARCH_STRING_MAX+1];
extern byte islocked;

/* from getR.as */
byte getR(void);

/* from memo_io.c */
int load_or_delete_memo(unsigned n,byte delete);
int write_file(void);
extern byte issecret;
extern byte num_recnums;
extern unsigned recnums[];
extern byte memo[HEADER+MAX_MEMO_LENGTH+1];
int readmemotitledate(unsigned n);

/* from stricmp.as */
int stricmp(char *s1,char *s2);

/* from ask.c */
unsigned warn(char *s,char *t);


/* etc */
int ozeditline1(byte _x0,byte y0,char *s0,byte slen,byte xlen);
unsigned ozgetw(void);
byte ozputs0(byte y,char *s);
void ozputsseq(byte y,char *s);

extern char yn_message[];
extern char anykey_message[];
extern byte modified0;
void fulfill_up_queue(void);
void fulfill_down_queue(void);
void clear_top(void);
void adjust_lines(int l, byte redraw);
void ifseldel(void);
byte import_file(void);
unsigned ozreadauxword2(unsigned offset);
unsigned getch_cursor(void);

extern struct head_struct
{
    unsigned long start_time;
    unsigned x0;
    unsigned x1;
} head0;

#define ozgetch ozgetchblank

byte measurechar(char c);
byte measurestring(char *s);
byte ozputsf(byte y);
char *time_string(void);
