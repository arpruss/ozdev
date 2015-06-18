@echo off
del ..\compiler\liboz.lib
copy oz.h ..\compiler
copy ozmenu.h ..\compiler
copy ozmemo.h ..\compiler
copy ozindex.h ..\compiler
copy ozgfx.h ..\compiler
copy ozinput.h ..\compiler
copy ozscreen.h ..\compiler
copy oztime.h ..\compiler
copy ozint.h ..\compiler
copy ozserial.h ..\compiler
copy ozfile.h ..\compiler
copy ozaux.h ..\compiler
copy ozmisc.h ..\compiler
copy ozfont.h ..\compiler
copy alloca.h ..\compiler
make all
copy liboz.lib ..\compiler
copy 16k.obj ..\compiler
copy 32k.obj ..\compiler

