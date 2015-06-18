void setcols(byte cols);
void beep(void);
void termputch(byte c);
void serputs(char *s);
void reset_term(void);
void ttyscroll(void);
void ttyclearregion(unsigned from,byte numbytes, byte firstmask,
    byte lastmask,byte numlines);
void clearregion(byte x1,byte y1,byte x2,byte y2);

char *utoa(unsigned x);

extern byte SerBufGet,SerBufPut;
extern byte echo;

#define XON 17
#define XOFF 19
