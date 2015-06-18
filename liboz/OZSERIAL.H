/* 6. SERIAL PORT I/O */

#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif

void ozsnap(void);
byte ozgetrxhandshaking(void);
void ozsetrxhandshaking(byte state);
void ozclearserialbuffer(void);
void ozdatabits(byte databits);
int ozserialgetc(void);
void ozsetbaud(unsigned baud);
unsigned ozgetbaud(void);
void ozstopbits(char stopbits);
void ozparity(byte parity);
byte ozgetlcr(void);
void ozsetlcr(byte lcr);
int ozserialin(void);
void ozserialout(char c);
void ozserinton(void);
void ozserintoff(void);
void _ozserinton(void);
void _ozserintoff(void);
/* for ozparity() */
#define NO_PARITY 0
#define ODD_PARITY 0x08
#define EVEN_PARITY 0x18
#define HIGH_PARITY 0x28
#define LOW_PARITY	0x38

/* for ozstopbits() */
#define ONE_STOP_BIT 0
#define TWO_STOP_BITS 4
#define ONE_POINT_FIVE_STOP_BITS 4

/* for ozbaud() */
#define BAUD76800 2
#define BAUD51200 3
#define BAUD38400 4
#define BAUD30720 5
#define BAUD25600 6
#define BAUD19200 8
#define BAUD9600 16
#define BAUD4800 32
#define BAUD2400 64
#define BAUD1200 128
#define BAUD600 256
#define BAUD300 512
#define BAUD150 1024

/* for ozdatabits() */
#define DATABITS5 0
#define DATABITS6 1
#define DATABITS7 2
#define DATABITS8 3

/* for ozxxxlcr() */
#define BREAKSTATE 0x40

