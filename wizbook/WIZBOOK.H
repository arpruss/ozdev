#define FILEBUF_LEN 7592 /* must be even and match setting in help.as */
#define MAX_BOOKMARKS 50 /* per file */
#define MAX_POSITIONS 40
#define MAX_CLINE 250 /* compressed line maximum length */
void putformatted(byte y,char *fmt,...);
unsigned ozwarn(char *s,char *t);
extern byte myfont;
byte ozputs00(char *s);
#define CHARTABLE_MAX 93
extern byte chartable[CHARTABLE_MAX];
extern byte ext_compare;
typedef char fname[MAX_FILENAMELEN+1];
extern char anykey[];
extern char yn[];
extern char directory[];
extern char error_header[];
extern char error_font[];
extern char error_dict[];
extern char error_seek[];
extern char error_read[];
#define NUM_SINGLES 122
#define SINGLE_START (256-NUM_SINGLES)
#define WIDTH 239
#define MAX_WORD_LENGTH 21
#define MAX_WORDS (NUM_SINGLES+254*6) /* maximum number of words in dictionary */
/* needs to be defined in vars2.as */
void start(void);
extern fname filename;
extern unsigned long last;
extern unsigned long bookmarks[MAX_BOOKMARKS];
extern unsigned long positions[MAX_POSITIONS];
extern byte indentnum;
extern byte positionptr;
extern byte numpositions;
extern byte numbookmarks;
extern byte bookmarkptr;
extern byte bookmarksmod;

void startloadheaders(void);
extern byte x;
extern byte line_height;
extern unsigned ver;
extern unsigned delta;
extern byte chartablesize;
extern byte myfont;
extern byte line_height;
extern byte font_height;
extern byte num_lines;
extern byte backandforth;
extern byte show_bar;
extern unsigned scrollbar_pos;
extern unsigned header_length;
extern unsigned numwords;
extern unsigned dictionary_length;
extern byte index_data_length;
extern unsigned index_length;
extern unsigned long length;
extern char bookmark_marker[];
extern unsigned char filebuf[];
extern unsigned long buffer_offset;
extern unsigned screen_offset;

byte forwardline(void);
byte backline(void);
byte search(char *s,byte wild);
unsigned mygetch(void);
extern char line_buf[];
void goto_pos(unsigned long _pos);
void dosearch_from(unsigned long start);
void searchtobookmarksfrom(unsigned long start);
void status(char *s);
unsigned ybar(byte spread);
extern unsigned long saved_last;
extern byte searching;
extern char error_msg[];
extern byte reseek;
void myseek(unsigned long pos);
typedef unsigned long (*getfun)(unsigned);
typedef void (*delfun)(unsigned);
unsigned toc(getfun get,delfun del,unsigned startpos,unsigned _num);
extern unsigned num_toc_entries;
extern unsigned start_toc;
void clearrest(byte y);
unsigned long trim(long pos);
extern unsigned long thepos;
void box239(byte y0,byte h,byte c);
void clear240(byte y0,byte h);
extern byte dorev;
byte ozputsf(byte y);
#define SEARCHBUF 57
#define MAX_SWORDS 6
extern char _ozspare1start[];
#define searchbuf _ozspare1start /* up to 116 bytes */
#define searchbuf_backup (_ozspare1start+SEARCHBUF+1)
void reload(unsigned len);
extern unsigned long position0;
int isalpha(char c);
int isdigit(char c);
int isalnum(char c);
int isalnum0(char c);
