#define MAX_GRAPHS 12
#define MAX_LINE 51

#ifdef DEF
#define E
#else
#define E extern
#endif

typedef char oneline[MAX_LINE];

E oneline lines[MAX_GRAPHS];
E char truncate_chars[MAX_GRAPHS];
E char *truncate_pos[MAX_GRAPHS];
E struct ozmenuentry graph_select[MAX_GRAPHS+1];
E byte numgraphs;
E byte interrupted;
E byte quiet;
E double value_x,value_y,value_r,value_h;
#define MISC_LEN 70
E char miscstring[MISC_LEN];
extern double x_min,x_max,y_min,y_max;
extern double h_min,h_max,t_min,t_max;
extern double h_delta,t_delta;
extern char anykey[];
extern int do_special;

byte graph(char *line);
byte compile(char *line);
unsigned ozwarn(char *s,char *t);
void trace(char *line);
void getoption(void);
byte getfperr(void);
void setfperr(byte n);

void zoom(double x,double y,double factor);
#define XOFY 0
#define YOFX 1
#define XYOFT 2
#define ROFH 3
#define RHOFT 4

#define TO_DEG 57.2957795131
#define FROM_DEG 0.01745329252
