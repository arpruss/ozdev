@echo off
echo Linking...
call sethtc.bat
echo -C8000h -o%1.bin -plowpage=8000h,midpage,text,data,bss -m%1.map %L%32k.obj %1.obj %2 %3 %4 %5 %6 %7 %8 %9 %L%libc0.lib %L%liboz.lib %L%libc0.lib %L%liboz.lib > link.arg
call xlink link.arg
del link.arg
echo Checking crucial symbol locations...
checksym %1.map __Hlowpage le a000 __Lhighpage ge c000 __Hmidpage le c000 __Hhighpage le f200 __Hbss le f980 __Lbeforebss ge c000 __Hbss print 0
if errorlevel 1 goto delfile
echo Creating %1.wzd...
makewzd %1
goto getout
:delfile
del %1.bin
:getout

