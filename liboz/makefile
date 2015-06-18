SUFFIXES = .obj .as .lib
CC = zcc -c -ox

.as.obj:
        zxas -x $*.as

.c.obj:
        $(CC) $*.c

all: liboz.lib 16k.obj 32k.obj

OBJS1=dmemo.obj ozfr2.obj ozfr.obj printf.obj spnum_p.obj ozdel64.obj \
ozdel64k.obj \
ozgedlin.obj ozgetl.obj ozpoke.obj tell1.obj lseek1.obj rewind1.obj lseek1s.obj lseek1ro.obj \
lseek.obj file.obj getfile.obj file1ro.obj part.obj \
file1.obj file1b.obj unlink.obj owner.obj hash.obj ozngetch.obj ozgclick.obj

OBJS2=ngetch.obj getch.obj ozkbdon.obj ozscroll.obj ozscr2.obj ozdelay.obj \
ozgputs.obj ozputs2.obj ftable.obj ozwrfb.obj ozwrfm.obj ozscf.obj ozscf0.obj ozputsor.obj ozorbyte.obj ozandbyt.obj ozputbyt.obj \
putchar.obj ozgfbox.obj ozpagecp.obj ozmenu.obj ozsavscn.obj \
ozcrc.obj ozauxmem.obj ozauxme0.obj ozauxbyt.obj ozauxwor.obj \

OBJS3=ozputch.obj ozport.obj ozkeyupr.obj ozsbrk.obj ozser.obj \
ozgpoint.obj ozprof.obj ozserctl.obj ozlight.obj ozglight.obj ozmisc.obj ozint2.obj \
oz_int.obj ozseri.obj ozsave.obj ozsavep.obj ozkeyspd.obj ozpower.obj ozpower2.obj ozgcls.obj \
ozfndrec.obj ozfi4.obj ozfi3.obj ozfi2.obj ozfile.obj ozt.obj oztime.obj ozscrn.obj

OBJS4=ozfont.obj ozsingle.obj ozgline.obj ozgcirc.obj ozggpt.obj \
ozcls.obj ozunlink.obj ozputsns.obj ozgetp.obj ozsound.obj \
ozcirc.obj ozcircsm.obj ozcustom.obj ozflash.obj ozsetaut.obj \
ozpage.obj ozclick.obj ozboxf.obj _ozbox.obj _ozboxf.obj ozsprite.obj

OBJS5=ozbox.obj ozinbyte.obj ozgetfh.obj newmul.obj memcpy.obj \
memset.obj sprintf.obj spnum.obj sscanf.obj ozwrite.obj \
ozfi1.obj ozfar.obj copy2pg.obj pcopy.obj pcopy0.obj ozexitto.obj ozgetchb.obj ozkey.obj ozidx.obj \
ozdetect.obj atexit.obj ozexitf.obj ozfsdirt.obj ozblank.obj ozcpkset.obj \
ozaux.obj ozgrey.obj getslot.obj

OBJS6=ozcontr.obj ozlcd.obj ozline.obj _ozline.obj \
ozcopy.obj ozindby.obj ozpoint.obj wrelop.obj  ozgetsl2.obj ozgetsl.obj \
_ozpoint.obj ozvline.obj ozhline.obj ozhscrol.obj ozapage.obj ozindnum.obj \
ozdiei.obj restpage.obj setpage1.obj alloca.obj ozauxsiz.obj ozsnap.obj \
ozsetfp.obj ozclrf.obj oz64hz.obj oz64hz2.obj settings.obj ozwipe.obj


file.c: file.h

file1.c: file.c file.h

ozngetch.obj: ozngetch.c nicegetc.c
        $(CC) ozngetch.c

ngetch.obj: ngetch.c nicegetc.c
        $(CC) ngetch.c

liboz.lib: $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6)
        -del liboz.lib
        -del ..\compiler\liboz.lib
        zxecho libargs r liboz.lib $(OBJS1)
        xlibr libargs
        zxecho libargs r liboz.lib $(OBJS2)
        xlibr libargs
        zxecho libargs r liboz.lib $(OBJS3)
        xlibr libargs
        zxecho libargs r liboz.lib $(OBJS4)
        xlibr libargs
        zxecho libargs r liboz.lib $(OBJS5)
        xlibr libargs
        zxecho libargs r liboz.lib $(OBJS6)
        xlibr libargs

