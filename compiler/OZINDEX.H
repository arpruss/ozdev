struct appindexlist
{
    unsigned unknown;
    unsigned indexes[3];
};

void ozgetappindexlist(byte typenum, struct appindexlist *ilist);
extern byte _ozindexbycounts[8];
byte ozgetindexby(byte typenum);
void ozsetindexby(byte typenum,byte value);

