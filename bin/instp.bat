@echo off
if %1!==! goto instr
if not exist %1\bin\hlink.exe goto instr
if not exist %1\bin\libr.exe goto instr
gsethtc
if errorlevel 1 goto end
call sethtc.bat
echo @echo off > %L%..\bin\xlink.bat
echo %1\bin\hlin1.exe @%%1 >> %L%..\bin\xlink.bat
echo @echo off > %L%..\bin\xlibr.bat
echofrom %1\bin\libr.exe %%1 >>%L%..\bin\xlibr.bat
copy %1\bin\hlink.exe %L%..\bin
copy %L%..\bin\hlin1.exe %1\bin
echo Pacific C tools installed into SDK.
goto end
:instr
echo This lets you use some tools from Pacific C which speeds up
echo some processes such as linking.  You can download Pacific C,
echo a freeware compiler, from www.htsoft.com, the same nice folks who
echo brought us the freeware Z80 compiler we're using.  After you
echo download it, make a fresh directory somewhere, and run PACIFIC.EXE
echo in that directory to uncompress it.  Then, to make the Hi-Tech C SDK
echo use components from Pacific C, run:
echo    INSTP "dir"
echo where "dir" specifies the directory that you ran PACIFIC.EXE in (e.g.,
echo C:\pacific or "c:\program files\wiz\pacific";  use quotation marks if
echo the directory has a space in it). If you want to save disk space, the
echo only files you need from Pacific C are bin\hlink.exe and bin\libr.exe
echo at the moment.
echo.
echo To deinstall the Pacific C tools from this SDK, run UNINSTP.
:end
