@echo off

REM Compile Library:
del lib3d.lib
zcc -c -ox lib3d.c isin.as
zxlibr r lib3d.lib lib3d.obj isin.obj

REM Compile Sample Program:
zcc -z -ox -q 3d.c lib3d.lib
makewzd 3d @desc.txt
