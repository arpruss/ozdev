@echo off
if not exist %1.c goto empty
if exist %1.aso del %1.aso
zxc -s %1.c
if not exist %1.as goto end
miniopt0 < %1.as |  miniopt | miniopt1 | miniopt2 > %1.aso
del %1.as
zxas -j %1.aso
goto end
:empty
echo File %1.c not found!
:end
