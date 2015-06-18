typedef unsigned int byte;
void ttyscroll(void);
void ttyclearregion(unsigned from,byte numbytes,
byte firstmask,byte lastmask,byte numlines);
void clearregion(byte x1,byte y1,byte x2,byte y2);
int serialget(void);
