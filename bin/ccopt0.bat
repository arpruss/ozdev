@echo off
rem EDIT COMPILER DIRECTORY IN NEXT LINE
if not exist c:\progra~1\wiz\hitech\compiler\oz.h goto baddir
if not exist %1.c goto empty
if exist %1.aso del %1.aso
if exist %1.obj del %1.obj
rem EDIT COMPILER DIRECTORY IN NEXT LINE
dcpp -DCPM -DHI_TECH_C -Dz80 -DOZ7xx %2 %3 %4 %5 %6 %7 %8 %9 -Ic:\progra~1\wiz\hitech\compiler %1.c> %1.i
if errorlevel 1 goto end
irrel %1.i
zxcc p1 %1.i %1.$2
if errorlevel 1 goto end
zxcc cgen %1.$2 %1.$3
if errorlevel 1 goto end
falloca <%1.$3 >%1.$4
if errorlevel 1 goto end
zxcc optim %1.$4 %1.as0
if errorlevel 1 goto end
miniopt0 <%1.as0 | miniopt | miniopt1 | miniopt2 > %1.aso
if errorlevel 1 goto end
zxas -X -j %1.aso
goto end
:empty
echo File %1.c not found!
goto the_end
:baddir
echo You need to edit ccopt0.bat and change the two lines preceded by the line
echo       rem EDIT COMPILER DIRECTORY IN NEXT LINE
echo to have the correct path to Hi-Tech C's COMPILER directory.
goto the_end
:end
rem if exist %1.i del %1.i
if exist %1.$2 del %1.$2
rem if exist %1.$3 del %1.$3
rem if exist %1.$4 del %1.$4
if exist %1.as0 del %1.as0
:the_end
