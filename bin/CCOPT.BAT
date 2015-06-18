@echo off
if not exist %1.c goto empty
if exist %1.aso del %1.aso
if exist %1.obj del %1.obj
zxc %2 %3 %4 %5 %6 %7 %8 %9 -DOZ7XX -s -o %1.c
if not exist %1.as goto end
miniopt0 < %1.as |  miniopt | miniopt1 | miniopt2 > %1.aso
del %1.as
zxas -j -x %1.aso
goto end
:empty
echo File %1.c not found!
:end
