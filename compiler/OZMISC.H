#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

#define ozis770() (MODEL770PC==ozdetectmodel())

void ozsetautorun(byte number);
byte ozgetautorun(void);

byte ozgetmembyte(unsigned page, unsigned offset);

void ozfarmemcpy(unsigned dest_page,unsigned dest_offset,unsigned src_page,
  unsigned src_offset,unsigned length);

void ozcopytopage(unsigned dest_page,unsigned dest_offset,byte *src,unsigned length);

void ozsound(unsigned div);
void ozquiet(void);

void ozexitto(unsigned key);
void ozfast(void);
void ozslow(void);

void ozkeyclick(void);

void ozdelay(unsigned d);
void exit(int ignored_exit_code);
byte ozportin(byte port);
void ozportout(byte port, byte value);

void ozsound(unsigned value);
void ozquiet(void);
void ozinitsound(void);



extern unsigned _oz1hz;
extern unsigned _oz64hz_word;
extern unsigned long _oz64hz_dword;



byte ozdetectmodel(void);

#define MODELMASK_MEMORY 1
#define MODELMASK_770	 2
#define MODELMASK_M 	 4
#define MODEL700PC 0
#define MODEL730PC 0
#define MODEL750PC MODELMASK_MEMORY
#define MODEL770PC (MODELMASK_MEMORY | MODELMASK_770)
#define MODEL700M  MODELMASK_M
#define MODEL750M  (MODELMASK_MEMORY | MODELMASK_M)
#define MODEL770M  (MODELMASK_MEMORY | MODELMASK_770 | MODEMASK_M)
					  /* 770M NOT YET EXISTENT OR DETECTED, BUT IF IT IS */
					  /* MADE, THAT WILL BE ITS MODEL NUMBER */

char *ozgetnextfilename(byte mode);
#define FIND_RESET 1
#define FIND_OWNED 2



byte ozgetpowerkeyhandling(void);
void ozsetpowerkeyhandling(byte state);

unsigned ozkeydelay64hz(unsigned len);
void ozdelay64hz(unsigned length);

void oz64hztimeron(void);
void oz64hztimeroff(void);
byte ozget64hztimerstate(void);
void ozinstallmemorywiper(void);

extern byte _ozprogoptions;
extern byte _ozclick_setting;

#define OZ_OPTION_SCROLL_FULL     1
#define OZ_OPTION_NO_SOUND        2
#define OZ_OPTION_KEY_AFTER_BLANK 4
#define OZ_OPTION_SHOW_CLOCK      8
#define OZ_OPTION_SHOW_DATE      16

byte ozgetstatus(void);
#define OZ_STATUS_CAPSLOCK 2
#define OZ_STATUS_LOCKED 16
#define OZ_STATUS_BACKLIGHT 64
#define OZ_STATUS_BATTERYLOW 128

#ifndef NULL
#define NULL ((void*)0)
#endif

extern struct
{
    byte operation;
    void (*function)(unsigned);
} _ozgetchhook;

char *ozgetprogramname(byte slot);
int ozgetslotnumber(char *name);
void ozrunslot(byte slot);
