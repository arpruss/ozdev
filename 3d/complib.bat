@echo off
REM -----------------------------
REM       COMPILE LIBRARIES
REM -----------------------------
zcc -c -ox lib3d.c
zxas -j -x isin.as
zxlibr r lib3d.lib lib3d.obj isin.obj
