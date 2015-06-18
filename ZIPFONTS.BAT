@echo off
echo If you are not Alexander Pruss, press ctrl-break.
pause
cd fonts
zip -9r ..\..\fonts *.*
cd ..\docs
zip -9r ..\..\fonts fonts.txt
cd ..\bin
zip -9r ..\..\fonts getmacfont.exe font2wiz.exe getprcfont.exe wzf2txt.exe txt2wzf.exe dfont.exe wzf2prog.exe tomemo.exe
copy ..\..\fonts.zip "\my documents\soft\dl"

