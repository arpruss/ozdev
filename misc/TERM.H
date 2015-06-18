void beep(void);
void myISR(void);
void termputch(byte c);
void serputs(char *s);
void reset_term(void);
void ttyscroll(void);
void ttyclearregion(unsigned from,byte numbytes, byte firstmask,
    byte lastmask,byte numlines);
void clearregion(byte x1,byte y1,byte x2,byte y2);
int serialget(void);
void keyclick(void);

#define MASKSHIFT 0x0800
#define MASKCTRL  0x0200
#define MASKCATEGORY 0x0400
#define MASK2ND   0x0100

extern byte SerBufGet,SerBufPut;

#define XON 17
#define XOFF 19
