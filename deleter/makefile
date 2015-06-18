all: deleter.wzd

OBJS2 = scaldate.obj

CCOPTS = -o


.as.obj:
        zxas $*.as

.c.obj:
        ccopt $*


deleter.wzd: deleter.obj $(OBJS2)
        ozlink32 deleter $(OBJS2)
        makewzd deleter "Deleter! 1.0a" @deleter.dsc
        ozpack deleter.wzd
