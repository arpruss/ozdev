all: dfont.exe font2wiz.exe wzf2prog.exe fonts.wzd getmacfont.exe \
wzf2txt.exe txt2wzf.exe getprcfont.exe

font2wiz.exe: font2wiz.c tomac.c xlat.c ascii2me.c

.c.exe:
        bcc32 -O2 -4 -X -v-  $*.c
        del $*.tds

OBJS2 = warn.obj putform.obj frommemo.obj

.as.obj:
        zxas $*.as

.c.obj:
        zcc -ox -c $*.c

fonts.obj: fonts.c fonts.h

fonts.wzd: fonts.obj $(OBJS2) fonts.inf
        ozlink fonts $(OBJS2)
        makewzd -999 fonts @fonts.inf
        ozpack fonts.wzd
