all: archess.wzd

OBJS2 = ARMOV.obj AREVAL.obj ARGEN.obj ARLIB.obj ARVAR.obj IO.obj IOLIB.obj \
IOSCREEN.obj IOSETHLP.obj SEARCH.obj SLIB.obj tc_port.obj scr8x8.obj putch.obj \
smallstr.obj misc.obj ozput.obj

archess.lib: $(OBJS2)
        zxecho lib.arg r archess.lib $(OBJS2)
        xlibr lib.arg

.c.obj:
        zcc -ox -c -DOZ7XX $*.c

b2c.exe: b2c.c
        cl b2c.c

pieces.as: pieces b2c.exe
        b2c < pieces > pieces.as

scr8x8.obj: scr8x8.as pieces.as copyrt.as
        zxas scr8x8.as

.as.obj:
        zxas $*.as

archess.wzd: archess.obj archess.lib archess.inf
        zcc -ziarchess.inf -z48 archess.obj archess.lib
        grep _Hbss archess.map
        ozpack archess.wzd
