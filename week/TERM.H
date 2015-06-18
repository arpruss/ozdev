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

extern byte SerBufGet,SerBufPut;

#define XON 17
#define XOFF 19
