@echo off
gsethtc
call sethtc.bat
echo @echo off > %L%..\bin\sethtc.bat
echo set L= >> %L%..\bin\sethtc.bat
echo @echo off > %L%..\bin\xlink.bat
echo zxlink @%%1 >> %L%..\bin\xlink.bat
echo @echo off > %L%..\bin\xlibr.bat
echofrom zxlibr %%1 >>%L%..\bin\xlibr.bat
del %L%..\bin\hlink.exe
echo Pacific C stuff uninstalled from Hi-Tech C package.
