/* 5. INTERRUPT HANDLING */
#ifndef _OZ_BYTE
typedef unsigned char byte;
#define _OZ_BYTE
#endif
void _ozcustomisr(void);
/* typedef void (*ozisr)(void); */
int ozsetisr(void *f);   /* set custom ISR with paging */
int _ozsetisr(void *f);  /* same, but no paging--routine must always
                            be paged in */
void ozisroff(void);      /* turn off custom ISR (set im 1) */
void ozdisableinterrupts(void);
void ozenableinterrupts(void);
void ozintwait(void);

#ifndef NULL
#define NULL ((void*)0)
#endif

